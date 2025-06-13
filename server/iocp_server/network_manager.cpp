#include "network_manager.h"

int get_new_client_id()
{

	for (int i = 0; i < MAX_USER; ++i) {
		auto it = object.find(i);
		if (it == object.end()) return i;
	}
	return -1;
}

void network_init()
{
	WSADATA WSAData;
	WSAStartup(MAKEWORD(2, 2), &WSAData);
	g_s_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	SOCKADDR_IN server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(GAME_PORT);
	server_addr.sin_addr.S_un.S_addr = INADDR_ANY;
	bind(g_s_socket, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr));
	listen(g_s_socket, SOMAXCONN);
	SOCKADDR_IN cl_addr;
	int addr_size = sizeof(cl_addr);

	h_iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
	CreateIoCompletionPort(reinterpret_cast<HANDLE>(g_s_socket), h_iocp, 10001, 0);
	g_c_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	g_a_over._comp_type = OP_ACCEPT;
	AcceptEx(g_s_socket, g_c_socket, g_a_over._send_buf, 0, addr_size + 16, addr_size + 16, 0, &g_a_over._over);

}

void network_close()
{
	closesocket(g_s_socket);
	WSACleanup();
}


void disconnect(int c_id)
{

	auto it = object.find(c_id);
	if (it == object.end()) return;
	std::shared_ptr<USER> c = std::dynamic_pointer_cast<USER>(it->second.load());
	
	delete_sector(c->_id, c->x, c->y);

	auto vl = c->_view_list;

	for (auto& p_id : vl) {
		auto it = object.find(p_id);
		if (it == object.end()) continue;
		shared_ptr<USER>pl = std::dynamic_pointer_cast<USER>(it->second.load());
		{
			lock_guard<mutex> ll(pl->_s_lock);
			if (ST_INGAME != pl->_state) continue;
		}
		if (pl->_id == c_id) continue;
		pl->send_remove_player_packet(c_id);
	}
	closesocket(c->_socket);

	lock_guard<mutex> ll(c->_s_lock);
	c->_state = ST_FREE;
}

void USER::do_send(void* packet)
{
	OVER_EXP* sdata = new OVER_EXP{ reinterpret_cast<char*>(packet) };
	WSASend(_socket, &sdata->_wsabuf, 1, 0, 0, &sdata->_over, 0);
}

void USER::send_move_packet(int c_id)
{

	auto it = object.find(c_id);
	if (it == object.end()) return;
	std::shared_ptr<USER> c = std::dynamic_pointer_cast<USER>(it->second.load());
	sc_packet_move p;
	p.id = c_id;
	p.size = sizeof(sc_packet_move);
	p.type = S2C_P_MOVE;
	p.x = c->x;
	p.y = c->y;
	do_send(&p);
}

void USER::send_remove_player_packet(int c_id)
{
	
	if (_view_list.count(c_id))
		_view_list.unsafe_erase(c_id);
	else {
		return;
	}
	
	sc_packet_leave p;
	p.id = c_id;
	p.size = sizeof(p);
	p.type = S2C_P_LEAVE;
	do_send(&p);
}

void USER::send_login_fail_packet(int reason)
{
	std::cout << "존재하지않는 아이디, 로그인실패" << std::endl;
	sc_packet_login_fail p;
	p.size = sizeof(p);
	p.type = S2C_P_LOGIN_FAIL;
	p.reason = reason;
	do_send(&p);
}

void USER::send_login_info_packet()
{
	sc_packet_avatar_info p;
	p.id = _id;
	p.size = sizeof(S2C_P_AVATAR_INFO);
	p.type = S2C_P_AVATAR_INFO;
	p.x = x;
	p.y = y;
	do_send(&p);
}

void USER::send_add_player_packet(int c_id)
{
	auto it = object.find(c_id);
	if (it == object.end()) return;
	std::shared_ptr<USER> c = std::dynamic_pointer_cast<USER>(it->second.load());
	sc_packet_enter add_packet;
	add_packet.id = c_id;
	strcpy_s(add_packet.name, c->_name);
	add_packet.size = sizeof(add_packet);
	add_packet.type = S2C_P_ENTER;
	add_packet.x = c->x;
	add_packet.y = c->y;
	_view_list.insert(c_id);
	do_send(&add_packet);
}

void USER::send_chat_packet(int c_id, const char* mess)
{
	sc_packet_chat packet;
	packet.id = c_id;
	packet.size = sizeof(packet);
	packet.type = S2C_P_CHAT;
	strcpy_s(packet.message, mess);
	do_send(&packet);
}
