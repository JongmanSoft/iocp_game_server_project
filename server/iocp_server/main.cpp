#include "stdafx.h"
#include "DB_Manager.h"
#include "network_manager.h"
#include "Timer_manager.h"
#include "ai_manager.h"

HANDLE h_iocp;

SOCKET g_s_socket, g_c_socket;
OVER_EXP g_a_over;


void load_view_list(int c_id) {
	auto it = object.find(c_id);
	if (it == object.end()) return;
	std::shared_ptr<USER> c = std::dynamic_pointer_cast<USER>(it->second.load());
	std::pair<int, int>index = get_sector_index(c->x, c->y);
	std::pair<int, int> min_index = { max(0,index.first - 1), max(0,index.second - 1) };
	std::pair<int, int> max_index = {
		min(MAP_HEIGHT / SECTOR_SIZE - 1,index.first + 1),
			min(MAP_WIDTH / SECTOR_SIZE - 1,index.second + 1) };

	for (int row = min_index.first; row < max_index.first + 1; row++) {
		for (int col = min_index.second; col < max_index.second + 1; col++) {
			concurrency::concurrent_unordered_set<int> local_list;
			{
				std::lock_guard<std::mutex> ll(g_sector_mutexes[row][col]);
				local_list = g_sectors[row][col];
			}

			for (const int& p_id : local_list) {
				shared_ptr<OBJECT>p = object.at(p_id);
				{
					lock_guard<mutex> ll(p->_s_lock);
					if (ST_INGAME != p->_state) continue;
				}
				if (p->_id == c_id) continue;
				if (false == can_see(c_id, p->_id))
					continue;
				if (is_pc(p->_id)) {
					shared_ptr <USER> PP = std::dynamic_pointer_cast<USER>(p);
					PP->send_add_player_packet(c_id);
				} 
				
				if (is_pc(p->_id)) {
					c->send_add_player_packet(p->_id);
				}
				else {

					c->send_add_object_packet(p->_id);
					if (can_see(p->_id, c_id))WakeUp(p->_id, c_id);
				}
			}
		}
	}
}

void update_view_list(int c_id) {
	auto it = object.find(c_id);
	if (it == object.end()) return;
	std::shared_ptr<USER> c = std::dynamic_pointer_cast<USER>(it->second.load());

	std::pair<int, int> centre_index = get_sector_index(c->x, c->y);
	unordered_set<int> near_list;
	std::pair<int, int> min_index = { max(0,centre_index.first - 1), max(0,centre_index.second - 1) };
	std::pair<int, int> max_index = { min(MAP_HEIGHT / SECTOR_SIZE - 1 ,centre_index.first + 1),min(MAP_WIDTH / SECTOR_SIZE - 1,centre_index.second + 1) };

	for (int row = min_index.first; row < max_index.first + 1; row++) {
		for (int col = min_index.second; col < max_index.second + 1; col++) {
			concurrency::concurrent_unordered_set<int> local_list;
			{
				std::lock_guard<std::mutex> ll(g_sector_mutexes[row][col]);
				local_list = g_sectors[row][col];
			}

			for (auto& cl : local_list) {
				shared_ptr<OBJECT>p = object.at(cl);
				if (p->_state != ST_INGAME) continue;
				if (p->_id == c_id) continue;
				if (can_see(c_id, p->_id))
					near_list.insert(p->_id);
			}
		}
	}

	auto old_vlist = c->_view_list;


	for (auto& pl : near_list) {
		
		auto it = object.find(pl); if (it == object.end()) break;
		shared_ptr <OBJECT> cpll = it->second.load();
		auto cpl = std::dynamic_pointer_cast<USER>(cpll);
		if (cpl) { //

			if (cpl->_view_list.count(c_id)) { //상대방뷰리스트에 내가 있다면
				cpl->send_move_packet(c_id);
			}
			else {
				cpl->send_add_player_packet(c_id);
			}
			
		
		}
		else { 
			auto npl = std::dynamic_pointer_cast<NPC>(cpll);
			WakeUp(npl->_id, c_id);
		}
		if (old_vlist.count(pl) == 0) {
			if (is_pc(pl))c->send_add_player_packet(pl);
			else c->send_add_object_packet(pl);
			if (is_npc(cpll->_id)) WakeUp(cpll->_id, c_id);
		}
	}
	for (auto& pl : old_vlist)
		if (0 == near_list.count(pl)) {
			auto it = object.find(pl); if (it == object.end()) break;
			shared_ptr <USER> cpl = std::dynamic_pointer_cast<USER>(it->second.load());
			c->send_remove_object_packet(pl);
			if (!cpl) {
				continue;
			}
			cpl->send_remove_object_packet(c_id);
		}
}

void update_animation(int c_id, int state, char dir) {
	auto it = object.find(c_id);
	if (it == object.end()) return;
	std::shared_ptr<OBJECT> c =it->second.load();
	std::pair<int, int>index = get_sector_index(c->x, c->y);
	std::pair<int, int> min_index = { max(0,index.first - 1), max(0,index.second - 1) };
	std::pair<int, int> max_index = {
		min(MAP_HEIGHT / SECTOR_SIZE - 1,index.first + 1),
			min(MAP_WIDTH / SECTOR_SIZE - 1,index.second + 1) };

	for (int row = min_index.first; row < max_index.first + 1; row++) {
		for (int col = min_index.second; col < max_index.second + 1; col++) {
			concurrency::concurrent_unordered_set<int> local_list;
			{
				std::lock_guard<std::mutex> ll(g_sector_mutexes[row][col]);
				local_list = g_sectors[row][col];
			}

			for (const int& p_id : local_list) {
				shared_ptr<OBJECT>p = object.at(p_id);
				{
					lock_guard<mutex> ll(p->_s_lock);
					if (ST_INGAME != p->_state) continue;
				}
				if (p->_id == c_id) continue;
				if (false == can_see(c_id, p->_id))
					continue;
				if (is_pc(p->_id)) {
					shared_ptr <USER> PP = std::dynamic_pointer_cast<USER>(p);
					PP->send_state_packet(c_id,state,dir);
					if (is_npc(c_id))PP->send_stat_packet(c_id);
				}
		
			}
		}
	}
}

void update_chat(int c_id, const char* mess) {
	auto it = object.find(c_id);
	if (it == object.end()) return;
	std::shared_ptr<USER> c = std::dynamic_pointer_cast<USER>(it->second.load());
	std::pair<int, int>index = get_sector_index(c->x, c->y);
	std::pair<int, int> min_index = { max(0,index.first - 1), max(0,index.second - 1) };
	std::pair<int, int> max_index = {
		min(MAP_HEIGHT / SECTOR_SIZE - 1,index.first + 1),
			min(MAP_WIDTH / SECTOR_SIZE - 1,index.second + 1) };

	for (int row = min_index.first; row < max_index.first + 1; row++) {
		for (int col = min_index.second; col < max_index.second + 1; col++) {
			concurrency::concurrent_unordered_set<int> local_list;
			{
				std::lock_guard<std::mutex> ll(g_sector_mutexes[row][col]);
				local_list = g_sectors[row][col];
			}

			for (const int& p_id : local_list) {
				shared_ptr<OBJECT>p = object.at(p_id);
				{
					lock_guard<mutex> ll(p->_s_lock);
					if (ST_INGAME != p->_state) continue;
				}
				if (p->_id == c_id) continue;
				if (false == can_see(c_id, p->_id))
					continue;
				if (is_pc(p->_id)) {
					shared_ptr <USER> PP = std::dynamic_pointer_cast<USER>(p);
					PP->send_chat_packet(c_id,mess);
				}

			}
		}
	}
}

void update_attack(int c_id, char action, char dir) {

	//c_id:행동을 한 유저, action:그 유저가 한 행동
	auto it = object.find(c_id);
	if (it == object.end()) return;
	std::shared_ptr<USER> c = std::dynamic_pointer_cast<USER>(it->second.load());
	std::pair<int, int>index = get_sector_index(c->x, c->y);
	std::pair<int, int> min_index = { max(0,index.first - 1), max(0,index.second - 1) };
	std::pair<int, int> max_index = {
		min(MAP_HEIGHT / SECTOR_SIZE - 1,index.first + 1),
			min(MAP_WIDTH / SECTOR_SIZE - 1,index.second + 1) };

	for (int row = min_index.first; row < max_index.first + 1; row++) {
		for (int col = min_index.second; col < max_index.second + 1; col++) {
			concurrency::concurrent_unordered_set<int> local_list;
			{
				std::lock_guard<std::mutex> ll(g_sector_mutexes[row][col]);
				local_list = g_sectors[row][col];
			}

			for (const int& p_id : local_list) {
				shared_ptr<OBJECT>p = object.at(p_id);
				{
					lock_guard<mutex> ll(p->_s_lock);
					if (ST_INGAME != p->_state) continue;
				}
				if (p->_id == c_id) continue;
				if (ACTION_ATTACK == action) {
					if (is_pc(p->_id))continue;
					if (false == can_attack(c_id, p->_id, dir))continue;
					//때리는데 성공함!
					shared_ptr <NPC> PP = std::dynamic_pointer_cast<NPC>(p);
					if (PP->_o_type != ORC_NPC) {
						std::cout << "<전투메세지>" << c_id << "가 " << p_id << "에게 " << Dammage(c->_level) << "데미지의 공격!" << std::endl;
						PP->_hp -= Dammage(c->_level);
						c->send_stat_packet(p->_id);
						if (PP->_hp <= 0) {
							std::cout << "<전투메세지>" << c_id << "가 " << p_id <<  "를 물리쳤다!" << std::endl;
							PP->_state = ST_FREE;
							delete_sector(p_id, PP->x, PP->y);
							update_animation(p_id, DEATH, npc_state_dir[dir]);
							c->_exp += NPC_EXP[c->_o_type - 2];
							if (c->_exp > need_next_level_exp(c->_level)) {
								//레벨업!
								int remain_exp = c->_exp - need_next_level_exp(c->_level);
								c->_level += 1;
								c->_exp = remain_exp;
							}
							c->send_stat_packet(c_id);
						}
						else {
							update_animation(p_id, HURT, npc_state_dir[dir]);
						}
					}
					
					
				}
				else if (ACTION_ATTACK_SKILL == action) {
					if (is_pc(p->_id))continue;
					if (false == can_skill(c_id, p->_id))continue;
					//때리는데 성공함!
					shared_ptr <NPC> PP = std::dynamic_pointer_cast<NPC>(p);
					if (PP->_o_type != ORC_NPC) {
						std::cout << "<전투메세지>" << c_id << "가 " << p_id << "에게 " << Dammage(c->_level) << "데미지의 전체공격!" << std::endl;
						PP->_hp -= Dammage(c->_level);
						c->send_stat_packet(p->_id);
						if (PP->_hp <= 0) {
							std::cout << "<전투메세지>" << c_id << "가 " << p_id << "를 물리쳤다!" << std::endl;
							PP->_state = ST_FREE;
							delete_sector(p_id, PP->x, PP->y);
							update_animation(p_id, DEATH, npc_state_dir[dir]);
							c->_exp += NPC_EXP[c->_o_type - 2];
							if (c->_exp > need_next_level_exp(c->_level)) {
								//레벨업!
								int remain_exp = c->_exp - need_next_level_exp(c->_level);
								c->_level += 1;
								c->_exp = remain_exp;
							}
							c->send_stat_packet(c_id);
						}
						else {
							update_animation(p_id, HURT, npc_state_dir[dir]);
						}
					}
				}
				else if (ACTION_HEAL_SKILL == action) {

					if (is_npc(p->_id))continue;
					if (false == can_skill(c_id, p->_id))continue;
					//회복스킬 성공함!
					shared_ptr <USER> PP = std::dynamic_pointer_cast<USER>(p);
						std::cout << "<전투메세지>" << c_id << "가 " << p_id << "에게 회복스킬 시전!" << std::endl;
						PP->_hp += 20;
						PP->_hp = min(MaxHP(PP->_level), PP->_hp);
						PP->send_stat_packet(p_id);
						DB_event DBE(DB_SAVE_HP, p_id);
						DBQ.push(DBE);
					
				}
				

			}
		}
	}
}

void init_npc() {
	for (int i = 0; i < 400; i++) {
		shared_ptr <NPC> p = make_shared<NPC>(MAX_USER+i,ORC_NPC,i);
		object.insert(make_pair(MAX_USER + i, p));
	}
}
//
bool User_InGame(std::string name) {
	for (const auto& user : object) {
		std::shared_ptr<OBJECT> u = user.second;
		if (is_npc(u->_id))return false;
		if (u->_name != name) continue;
		if (u->_state == ST_INGAME) return true;
		else break;
	}
	return false;
}

void process_packet(int c_id, char* packet) {
	auto it = object.find(c_id);
	if (it == object.end()) return;
	std::shared_ptr<USER> c = std::dynamic_pointer_cast<USER>(it->second.load());
	switch (packet[1]) {
	case C2S_P_LOGIN: {
		cs_packet_login* p = reinterpret_cast<cs_packet_login*>(packet);
		if (hasSpecialChar(p->name)) { c->send_login_fail_packet(2); break; }
		if (User_InGame(p->name)) { c->send_login_fail_packet(1); break; }
		if (get_new_client_id() == -1) { c->send_login_fail_packet(3); break; }
		strcpy_s(c->_name, p->name);
		DB_event dev = { DB_LOAD_INFO, c_id };
		DBQ.push(dev);
		ZeroMemory(packet, sizeof(cs_packet_login));
		break;
	}
	case C2S_P_MOVE: {
		cs_packet_move* p = reinterpret_cast<cs_packet_move*>(packet);
		short x = c->x;
		short y = c->y;
		switch (p->direction) {
		case MOVE_UP: if (y > 0) y--; break;
		case MOVE_DOWN: if (y < MAP_HEIGHT - 1) y++; break;
		case MOVE_LEFT: if (x > 0) x--; break;
		case MOVE_RIGHT: if (x < MAP_WIDTH - 1) x++; break;
		}
		if (get_sector_index(c->x, c->y) == get_sector_index(x, y))
		{
			c->x = x; c->y = y;
		}
		else {
			delete_sector(c->_id, c->x, c->y);
			c->x = x; c->y = y;
			insert_sector(c->_id, c->x, c->y);
		}
	
		DB_event dev = { DB_SAVE_XY, c_id };
		DBQ.push(dev);

		c->send_move_packet(c_id);
		update_view_list(c_id);
		}
				break;
	case C2S_P_STATE:{
		cs_packet_state* p = reinterpret_cast<cs_packet_state*>(packet);
		update_animation(c_id, p->state, p->direction);
	
		}
		break;

	case C2S_P_CHAT: {
		cs_packet_chat* p = reinterpret_cast<cs_packet_chat*>(packet);
		c->send_chat_packet(c_id, p->message);
		update_chat(c_id,p->message);
	}
		break;
	case C2S_P_ATTACK: {
		if (c->_able_attack) {
			cs_packet_attack* p = reinterpret_cast<cs_packet_attack*>(packet);
			//클라방향을 받음...서버방향으로 변환해서 주자
			int server_dir[4] = {MOVE_DOWN,MOVE_UP,MOVE_LEFT,MOVE_RIGHT};
			c->send_state_packet(c_id, ATTACK, server_dir[p->direction]);
			update_animation(c_id, ATTACK, server_dir[p->direction]);
			bool old_state = true;
			if (!atomic_compare_exchange_strong(&c->_able_attack, &old_state, false))break;
			TIMER_EVENT ev(c_id,1,attack_update,0);
			TIQ.push(ev);
		}
		break;
	}
	}
	
}


void worker_thread(HANDLE h_iocp)
{
	
	while (true) {
		DWORD num_bytes;
		ULONG_PTR key;
		WSAOVERLAPPED* over = nullptr;
		BOOL ret = GetQueuedCompletionStatus(h_iocp, &num_bytes, &key, &over, INFINITE);
		OVER_EXP* ex_over = reinterpret_cast<OVER_EXP*>(over);
		if (FALSE == ret) {
			if (ex_over->_comp_type == OP_ACCEPT) cout << "Accept Error";
			else {
				cout << "GQCS Error on client[" << key << "]\n";
				disconnect(static_cast<int>(key));
				if (ex_over->_comp_type == OP_SEND) delete ex_over;
				continue;
			}
		}

		if ((0 == num_bytes) && ((ex_over->_comp_type == OP_RECV) || (ex_over->_comp_type == OP_SEND))) {
			disconnect(static_cast<int>(key));
			if (ex_over->_comp_type == OP_SEND) delete ex_over;
			continue;
		}



		switch (ex_over->_comp_type) {
		case OP_ACCEPT: {
			int client_id = get_new_client_id();
			CreateIoCompletionPort(reinterpret_cast<HANDLE>(g_c_socket),
				h_iocp, client_id, 0);
			shared_ptr <USER> p = make_shared<USER>(client_id, g_c_socket, ST_ALLOC);
			object.insert(make_pair(client_id, p));
			p->do_recv();
			g_c_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
			ZeroMemory(&g_a_over._over, sizeof(g_a_over._over));
			int addr_size = sizeof(SOCKADDR_IN);
			AcceptEx(g_s_socket, g_c_socket, g_a_over._send_buf, 0, addr_size + 16, addr_size + 16, 0, &g_a_over._over);
			break;
		}
		case OP_RECV: {
			auto it = object.find(key);
			if (it == object.end()) break;
			shared_ptr <USER> c = std::dynamic_pointer_cast<USER>(it->second.load());
			int remain_data = num_bytes + c->_prev_remain;
			char* p = ex_over->_send_buf;
			while (remain_data > 0) {
				int packet_size = p[0];
				if (packet_size <= remain_data) {
					process_packet(static_cast<int>(key), p);
					p = p + packet_size;
					remain_data = remain_data - packet_size;
				}
				else break;
			}
			c->_prev_remain = remain_data;
			if (remain_data > 0) {
				memcpy(ex_over->_send_buf, p, remain_data);
			}
			c->do_recv();
			break;
		}
		case OP_SEND:
			delete ex_over;
			break;
		case OP_DB_LOAD_USER: {

			std::wcout << L"Existing account: ID=" << ex_over->result_info.uid
				<< L", X=" << (int)ex_over->result_info.x << L", Y=" << (int)ex_over->result_info.y
				<< L", Level=" << (int)ex_over->result_info.level << L", HP=" << (int)ex_over->result_info.hp << std::endl;
			auto it = object.find(key);
			if (it == object.end()) break;
			shared_ptr <USER> c = std::dynamic_pointer_cast<USER>(it->second.load());
			{
				lock_guard<mutex> ll{ c->_s_lock };
				c->x = ex_over->result_info.x;
				c->y = ex_over->result_info.y;
				c->_level = ex_over->result_info.level;
				c->_hp = ex_over->result_info.hp;
				c->_state = ST_INGAME;
				insert_sector(c->_id, c->x, c->y);
			}
			c->send_login_info_packet();
			load_view_list(c->_id);
			if (c->_hp < MaxHP(c->_level)) {
				TIMER_EVENT ev(key, 5, heal_update, 0);
				TIQ.push(ev);
			}
		}
			break;
		case OP_DB_NEW_USER: {
			std::wcout << L"NEW_USER!" << std::endl;
			auto it = object.find(key);
			if (it == object.end()) break;
			shared_ptr <USER> c = std::dynamic_pointer_cast<USER>(it->second.load());
			{
				lock_guard<mutex> ll{ c->_s_lock };
				c->x = init_x;
				c->y = init_y;
				c->_level = 1;
				c->_hp = 100;
				c->_state = ST_INGAME;
				insert_sector(c->_id, c->x, c->y);
			}
			c->send_login_info_packet();
			load_view_list(c->_id);
			DB_event dev = { DB_CREATE_USER, c->_id };
			DBQ.push(dev);

		}
			break;
		case OP_AI_HELLO:
			{
			auto it = object.find(key);
			if (it == object.end()) break;
			shared_ptr <NPC> c = std::dynamic_pointer_cast<NPC>(it->second.load());
			c->_ll.lock();
			auto L = c->_L;
			lua_getglobal(L, "event_hello");
			lua_pushnumber(L, ex_over->_ai_target_obj);
			lua_pcall(L, 1, 0, 0);
			c->_ll.unlock();
			delete ex_over;
			}
			break;
		//player 주기 
		case OP_PLAYER_ATTACK: {
			auto it = object.find(key);
			if (it == object.end()) break;
			shared_ptr <USER> c = std::dynamic_pointer_cast<USER>(it->second.load());
			bool old_state = false;
			if (!atomic_compare_exchange_strong(&c->_able_attack, &old_state, true)) break;
			//다시 공격할 수 있음!
			break;
		}
		case OP_PLAYER_SKILL: {
			auto it = object.find(key);
			if (it == object.end()) break;
			shared_ptr <USER> c = std::dynamic_pointer_cast<USER>(it->second.load());
			bool old_state = false;
			if (!atomic_compare_exchange_strong(&c->_able_attack_skill, &old_state, true)) break;
			break;
		}
		case OP_PLAYER_HEAL_SKILL: {
			auto it = object.find(key);
			if (it == object.end()) break;
			shared_ptr <USER> c = std::dynamic_pointer_cast<USER>(it->second.load());
			bool old_state = false;
			if (!atomic_compare_exchange_strong(&c->_able_heal_skill, &old_state, true)) break;
			break;
		}
		case OP_PLAYER_HEAL: {
			auto it = object.find(key);
			if (it == object.end()) break;
			shared_ptr <USER> c = std::dynamic_pointer_cast<USER>(it->second.load());
			c->_hp += (MaxHP(c->_level) * 0.1);
			c->_hp = min(c->_hp, MaxHP(c->_level));
			c->send_stat_packet(key);
			DB_event dev = { DB_SAVE_HP, key };
			DBQ.push(dev);
			if (c->_hp < MaxHP(c->_level)) {
				TIMER_EVENT ev(key, 5, heal_update, 0);
				TIQ.push(ev);
			}
			break;
		}
		case OP_PLAYER_DAMMAGE: {
			auto it = object.find(ex_over->_ai_target_obj);
			if (it == object.end()) break;
			shared_ptr <OBJECT> npc =it->second.load();
			if (npc->_o_type == S_HUMAN) {
				update_animation(npc->_id, ATTACK, 0);
			}

			break;
		}
		}
	}

}
 
int main() {

	
	DB_init();
	ai_init();
	network_init();
	//init_npc();

	vector <thread> worker_threads;
	int num_threads = std::thread::hardware_concurrency();
	for (int i = 0; i < num_threads; ++i)
		worker_threads.emplace_back(worker_thread, h_iocp);

	thread timer_thread{ Do_Timer };
	thread db_thread{ DB_thread };

	std::cout << "준비끝!" << std::endl;

	timer_thread.join();
	db_thread.join();

	for (auto& th : worker_threads)
		th.join();

	return 0;
}


