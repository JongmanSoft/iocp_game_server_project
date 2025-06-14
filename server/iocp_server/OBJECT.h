#pragma once
#include <mutex>
#include <memory>
#include "SECTOR.h"
#include "protocol.h"
#include "OVER_EXP.h"
#include "include/lua.hpp"
#pragma comment(lib, "lua54.lib")

//class OVER_EXP;

enum S_STATE { ST_FREE, ST_ALLOC, ST_INGAME };
class OBJECT
{
public:
	int _id;
	std::mutex _s_lock;
	S_STATE _state;
	short	x, y;
	char	_name[MAX_ID_LENGTH];

public:
	OBJECT() :_id{ -1 }, _state{ ST_FREE }, x{ 0 }, y{0} {};
	OBJECT(int id, S_STATE s) :_id{ id }, _state{ s }, x{ 0 }, y{ 0 } {};
	virtual ~OBJECT() = default;
};

class USER :public OBJECT {
public:
	SOCKET _socket;
	OVER_EXP _recv_over;
	int		_prev_remain;
	concurrency::concurrent_unordered_set <int> _view_list;
	int		last_move_time;

public:
	int _level =1;
	int _hp = 100;
	int _exp = 0;
public:
	USER(int new_id, SOCKET new_s, S_STATE new_st) :OBJECT(new_id,new_st), _socket{ new_s }
	{
		last_move_time = 0;
		_name[0] = 0;
		_prev_remain = 0;
	}

	void do_recv();
	void do_send(void* packet);
	void send_move_packet(int c_id);
	void send_remove_player_packet(int c_id);
	void send_state_packet(int c_id,int state,char dir);

	void send_login_fail_packet(int reason);
	void send_login_info_packet();
	void send_add_player_packet(int c_id);
	void send_chat_packet(int c_id, const char* mess);
	
};

class NPC : public OBJECT {

public:
	lua_State* _L;
	std::mutex	_ll;
	std::atomic_bool	_is_active;

public:
	NPC(int id) {
		//npc들만 쓸 생성자
		x = rand() % MAP_WIDTH;
		y = rand() % MAP_HEIGHT;
		_id = id;
		sprintf_s(_name, "NPC%d", id);
		_state = ST_INGAME;
		_is_active = false;
		insert_sector(_id, x, y); //wake up할때 생각하면 npc도 섹터에 넣어야,,,
		auto L = _L = luaL_newstate();
		luaL_openlibs(L);
		luaL_loadfile(L, "npc.lua");
		lua_pcall(L, 0, 0, 0);

		lua_getglobal(L, "set_uid");
		lua_pushnumber(L, _id);
		lua_pcall(L, 1, 0, 0);
		lua_pop(L, 1);// eliminate set_uid from stack after call
	}

};