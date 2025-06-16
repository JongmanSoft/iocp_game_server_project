#pragma once
#include "stdafx.h"

extern std::vector<std::vector<bool>> collision_data;

void ai_init();

void WakeUp(int npc_id, int waker);

char ai_random_move(int npc_id);

char ai_follow_move(int npc_id, int target_id);

bool keep_alive(int npc_id);

int api_send_hello(lua_State* L);

int api_get_x(lua_State* L);

int api_get_y(lua_State* L);
