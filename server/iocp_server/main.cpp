#include "stdafx.h"
#include "DB_Manager.h"
#include "network_manager.h"


HANDLE h_iocp;

SOCKET g_s_socket, g_c_socket;
OVER_EXP g_a_over;

void process_packet(int c_id, char* packet) {

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

			shared_ptr <USER> c = std::dynamic_pointer_cast<USER,OBJECT>(it->second.load());
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
		
		
		}
	}

}

int main() {

	return 0;
}