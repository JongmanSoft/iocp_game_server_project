#include "OBJECT.h"


void USER::do_recv()
{
	/*DWORD recv_flag = 0;
	memset(&_recv_over._over, 0, sizeof(_recv_over._over));
	_recv_over._wsabuf.len = BUF_SIZE - _prev_remain;
	_recv_over._wsabuf.buf = _recv_over._send_buf + _prev_remain;
	WSARecv(_socket, &_recv_over._wsabuf, 1, 0, &recv_flag,
		&_recv_over._over, 0);*/
}

void USER::do_send(void* packet)

{
	/*OVER_EXP* sdata = new OVER_EXP{ reinterpret_cast<char*>(packet) };
	WSASend(_socket, &sdata->_wsabuf, 1, 0, 0, &sdata->_over, 0);*/
}

void USER::send_move_packet(int c_id)
{
	/*std::shared_ptr<USER> c = std::dynamic_pointer_cast<USER,OBJECT>(object.at(c_id));
	sc_packet_move p;
	p.id = c_id;
	p.size = sizeof(sc_packet_move);
	p.type = S2C_P_MOVE;
	p.x = c->x;
	p.y = c->y;
	do_send(&p);*/
}

void USER::send_remove_player_packet(int c_id)
{
}
