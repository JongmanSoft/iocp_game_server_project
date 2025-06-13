#include "network_manager.h"

int get_new_client_id()
{

	for (int i = 0; i < MAX_USER; ++i) {
		auto it = object.find(i);
		if (it == object.end()) return i;
	}
	return -1;
}


void disconnect(int c_id)
{
	shared_ptr<USER>c = std::dynamic_pointer_cast<USER,OBJECT>(object.at(c_id));
	
	delete_sector(c->_id, c->x, c->y);

	auto vl = c->_view_list;

	for (auto& p_id : vl) {
		shared_ptr<USER>pl = std::dynamic_pointer_cast<USER,OBJECT>(object.at(p_id));
		{
			shared_ptr<USER>pl = std::dynamic_pointer_cast<USER, OBJECT>(object.at(c_id));
			if (ST_INGAME != pl->_state) continue;
		}
		if (pl->_id == c_id) continue;
		pl->send_remove_player_packet(c_id);
	}
	closesocket(c->_socket);

	lock_guard<mutex> ll(c->_s_lock);
	c->_state = ST_FREE;
}