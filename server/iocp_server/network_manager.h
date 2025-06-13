#pragma once
#include "stdafx.h"
#include "OVER_EXP.h"
#include "global_data.h"

extern HANDLE h_iocp;

extern SOCKET g_s_socket, g_c_socket;
extern OVER_EXP g_a_over;

int get_new_client_id();

void network_init();
void network_close();

void disconnect(int c_id);
