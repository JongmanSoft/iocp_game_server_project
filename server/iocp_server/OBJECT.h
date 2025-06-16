#pragma once
#include <mutex>
#include <memory>
#include "SECTOR.h"
#include "protocol.h"
#include "OVER_EXP.h"
#include "ai_manager.h"
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
	int _hp = 100;
	int _level = 1;
	int _exp = 0;
	char	_name[MAX_ID_LENGTH];
	int _o_type;

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
	

	std::atomic_bool _able_attack = true; //공격가능
	std::atomic_bool _able_attack_skill = true; //공격스킬가능
	std::atomic_bool _able_heal_skill = true; //회복스킬가능
public:
	USER(int new_id, SOCKET new_s, S_STATE new_st) :OBJECT(new_id,new_st), _socket{ new_s }
	{
		last_move_time = 0;
		_name[0] = 0;
		_prev_remain = 0;
		_o_type = PLAYER;
	}

	void do_recv();
	void do_send(void* packet);
	void send_move_packet(int c_id);
	void send_remove_object_packet(int c_id);
	void send_stat_packet(int c_id);
	void send_state_packet(int c_id,int state,char dir);

	void send_login_fail_packet(int reason);
	void send_login_info_packet();
	void send_add_player_packet(int c_id);
	void send_add_object_packet(int c_id);
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
	NPC(int id, int o_type, int zone) {
		_o_type = o_type;
		//npc들만 쓸 생성자
		_id = id;
		if (o_type == ORC_NPC)sprintf_s(_name, "%d번 오크", id);
		if (o_type == HUMAN)sprintf_s(_name, "%d번 마을사람", id),_hp = NPC_MAX_HP[0];
		if (o_type == S_HUMAN)sprintf_s(_name, "%d번 전사", id), _hp = NPC_MAX_HP[1];
		_state = ST_INGAME;
		_is_active = false;
		// Lua 상태 초기화
		auto L = _L = luaL_newstate();
		luaL_openlibs(L);
		if (luaL_loadfile(L, "npc.lua") || lua_pcall(L, 0, 0, 0)) {
			printf("Lua 파일 로드 실패: %s\n", lua_tostring(L, -1));
			lua_pop(L, 1);
			return;
		}

		// set_uid 호출
		lua_getglobal(L, "set_uid");
		if (!lua_isfunction(L, -1)) {
			lua_pop(L, 1);
			return;
		}
		lua_pushnumber(L, _id);
		if (lua_pcall(L, 1, 0, 0)) {
			lua_pop(L, 1);
			return;
		}


		lua_newtable(L); // 외부 테이블
		int collision_table_idx = lua_gettop(L); // 테이블의 스택 인덱스 저장
		for (size_t i = 0; i < collision_data.size(); ++i) {
			lua_pushnumber(L, i + 1); // Lua 인덱스 1-based
			lua_newtable(L); // 내부 테이블
			for (size_t j = 0; j < collision_data[i].size(); ++j) {
				lua_pushnumber(L, j + 1);
				lua_pushboolean(L, collision_data[i][j]);
				lua_settable(L, -3); // 내부 테이블에 설정
			}
			lua_settable(L, -3); 
		}

		lua_getglobal(L, "set_init_npc_");
		if (!lua_isfunction(L, -1)) {
			lua_pop(L, 1);
			return;
		}
		lua_pushnumber(L, o_type); // o_type
		lua_pushnumber(L, zone); // zone (임시)
		lua_pushvalue(L, collision_table_idx); // collision_array 테이블 복사
		if (lua_pcall(L, 3, 2, 0)) {
			lua_pop(L, 1);
			lua_pop(L, 1); // collision_array 테이블 제거
			return;
		}

		// 반환값 처리
		if (lua_isnumber(L, -2) && lua_isnumber(L, -1)) {
			x = static_cast<int>(lua_tonumber(L, -2));
			y = static_cast<int>(lua_tonumber(L, -1));
		}
		
		lua_pop(L, 2); // 반환값 제거
		lua_pop(L, 1); // collision_array 테이블 제거

		insert_sector(_id, x, y); 
	
		lua_register(L, "api_sendHello", api_send_hello);
		lua_register(L, "api_get_x", api_get_x);
		lua_register(L, "api_get_y", api_get_y);
	}

	
};