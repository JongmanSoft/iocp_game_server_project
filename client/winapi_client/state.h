#pragma once
#include "stdafx.h"
#include "sprite_data.h"
class state
{
public:
	int max_frame;
	int current_frame =0;
	int dir =0;
	void* owner;
	CImage* img;
public:
	state(int type,int state,void* object) {
		
		owner = object;
		switch (type)
		{
		case PLAYER:
			switch (state)
			{
			case IDLE:
				max_frame = 4;
				img = &sprite_data::get_inst().orc_idle;
				break;
			case WALK:
				max_frame = 6;
				img =&sprite_data::get_inst().orc_walk;
				break;
			case ATTACK:
				max_frame = 8;
				img = &sprite_data::get_inst().orc_attack;
				break;
			case DEATH:
				max_frame = 8;
				img =&sprite_data::get_inst().orc_death;
				break;
			case HURT:
				max_frame = 6;
				img = &sprite_data::get_inst().orc_hurt;
				break;
			default:
				break;
			}
			break;
		case ORC_NPC:
			switch (state)
			{
			case IDLE:
				max_frame = 4;
				img = &sprite_data::get_inst().orc_npc_idle;
				break;
			default:
				break;
			}
			break;
		case HUMAN:
			switch (state)
			{
			case IDLE:
				max_frame = 4;
				img = &sprite_data::get_inst().human_idle;
				break;
			case WALK:
				max_frame = 6;
				img = &sprite_data::get_inst().human_walk;
				break;
			case DEATH:
				max_frame = 7;
				img = &sprite_data::get_inst().human_death;
				break;
			case HURT:
				max_frame = 5;
				img = &sprite_data::get_inst().human_hurt;
				break;
			default:
				break;
			}
			break;
		case S_HUMAN:
			switch (state)
			{
			case IDLE:
				max_frame = 4;
				img = &sprite_data::get_inst().s_human_idle;
				break;
			case WALK:
				max_frame = 6;
				img = &sprite_data::get_inst().s_human_walk;
				break;
			case ATTACK:
				max_frame = 8;
				img = &sprite_data::get_inst().s_human_attack;
				break;
			case DEATH:
				max_frame = 7;
				img = &sprite_data::get_inst().s_human_death;
				break;
			case HURT:
				max_frame = 5;
				img = &sprite_data::get_inst().s_human_hurt;
				break;
			default:
				break;
			}
			break;
		default:
			break;
		}
	};
	int render_pos_get(int pos) {
		int r = MAP_CULLING/2;
		if (pos < MAP_CULLING / 2) r = pos;
		if (pos > (MAP_SIZE - MAP_CULLING / 2)) r = MAP_CULLING-((MAP_SIZE) - pos);
		return r;
	}
	void render(HDC m_hBufferDC, int x, int y) {

		int render_x = render_pos_get(x);
		int render_y = render_pos_get(y);

	img->AlphaBlend(m_hBufferDC,render_x*32-16,render_y*32-16,64,64,
	current_frame*64,dir*64,64,64, 255, AC_SRC_OVER);
	};
	virtual void update() = 0;
};

class loop_state : public state {
public:
	loop_state(int type, int state, void* object) : state(type, state, object) {
	}
	virtual void update() override;
};

class return_state : public state {
public:
	return_state(int type, int state, void* object) : state(type, state, object) {
	}
	virtual void update() override;
};

class end_state : public state {
public:
	end_state(int type, int state, void* object) : state(type, state, object) {
	}
	virtual void update() override;
};