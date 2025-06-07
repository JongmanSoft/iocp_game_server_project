#include "object.h"

void object::draw(HDC m_hBufferDC)
{
	state_ptr->render(m_hBufferDC, _x, _y);
}

void object::change_state(int state_type, int dir)
{
	int curr_frame = state_ptr->current_frame;
	switch (state_type)
	{
	case IDLE:
		state_ptr = std::make_unique<loop_state>(_type, state_type, this);
		break;
	case WALK:
		state_ptr = std::make_unique<return_state>(_type, state_type, this);
		state_ptr->current_frame = curr_frame;
		break;
	case ATTACK:
		state_ptr = std::make_unique<return_state>(_type, state_type, this);
		break;
	case HURT:
		state_ptr = std::make_unique<return_state>(_type, state_type, this);
		break;
	case DEATH:
		state_ptr = std::make_unique<end_state>(_type, state_type, this);
		break;
	default:
		break;
	}
	state_ptr->dir = dir;
}
