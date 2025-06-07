#pragma once
#include "stdafx.h"
#include "state.h"
class object
{
public:
	int _id;
	int _x, _y;
	char _name[20] = "jongman";//추후 프로토콜에 있는 네임사이즈로..
	int _hp = 100;
	int _level = 1;

public:
	//state관리
	int _state = -1;
	int _type;
	std::unique_ptr <state> state_ptr;
public:
	void draw(HDC m_hBufferDC);
	void change_state(int state_type, int dir);
public:
	object(int type, int x, int y) :_type{ type }, _x{ x }, _y{ y } {
		state_ptr = std::make_unique<loop_state>(PLAYER, IDLE, this);
	};

};

