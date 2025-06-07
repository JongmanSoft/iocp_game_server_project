#include "state.h"
#include "object.h"

void loop_state::update()
{
	current_frame++;
	if (current_frame >= max_frame) current_frame = 0;
}

void return_state::update()
{
	current_frame++;
	if (current_frame >= max_frame) {
		object* owner_obj = (object*)owner;
		owner_obj->change_state(IDLE, dir);
	}
}

void end_state::update()
{
	if(current_frame < max_frame-1)current_frame++;
}
