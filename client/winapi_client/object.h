#pragma once
#include "stdafx.h"
#include "state.h"
#include "chat_mess.h"
class object
{
public:
	int _id;
	int _x, _y;
	char _name[MAX_ID_LENGTH] = "jongman";
	int _hp = 100;
	int _level = 1;
	int _exp = 0;

public:
	//state����
	int _state = -1;
	int _type;
	std::unique_ptr <state> state_ptr;
	std::unique_ptr <chat_mess> mess_ptr;
public:
	void draw(HDC m_hBufferDC);
	void draw(HDC m_hBufferDC, int px, int py);
	void update();
	void change_state(int state_type, int dir);
public:
	object(int type, int x, int y) :_type{ type }, _x{ x }, _y{ y } {
		state_ptr = std::make_unique<loop_state>(PLAYER, IDLE, this);
		mess_ptr = std::make_unique<chat_mess>();
	};
	object(int type, int x, int y,int level , int hp,char name[MAX_ID_LENGTH]) :_type{type}, _x{x}, _y{y}, _level{level}, _hp{hp} {
		strcpy_s(_name, name);
		state_ptr = std::make_unique<loop_state>(PLAYER, IDLE, this);
		mess_ptr = std::make_unique<chat_mess>();
	};
	object(int type, int x, int y, char name[MAX_ID_LENGTH]) :_type{ type }, _x{ x }, _y{ y } {
		strcpy_s(_name, name);
		state_ptr = std::make_unique<loop_state>(type, IDLE, this);
		mess_ptr = std::make_unique<chat_mess>();
	};
};

