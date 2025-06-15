#pragma once
#include "stdafx.h"

extern std::vector<std::vector<bool>> collision_data;

void ai_init();

void WakeUp(int npc_id, int waker);

int api_send_hello(lua_State* L);

int api_get_x(lua_State* L);

int api_get_y(lua_State* L);
