#include "sprite_data.h"

void sprite_data::init()
{
	orc_idle	  .Load(L"image/orc/idle.png");
	orc_walk	  .Load(L"image/orc/walk.png");
	orc_hurt	  .Load(L"image/orc/hurt.png");
	orc_attack	  .Load(L"image/orc/attack.png");
	orc_death	  .Load(L"image/orc/death.png");
    orc_npc_idle  .Load(L"image/orc_npc/idle.png");
	human_idle.    Load(L"image/human/idle.png");
	human_walk.    Load(L"image/human/walk.png");
	human_hurt.   Load(L"image/human/hurt.png");
	human_death.   Load(L"image/human/death.png");
	s_human_idle. Load(L"image/s_human/idle.png");
	s_human_walk. Load(L"image/s_human/walk.png");
	s_human_hurt.Load(L"image/s_human/hurt.png");
	s_human_death.Load(L"image/s_human/death.png");
	s_human_attack.Load(L"image/s_human/attack.png");
}
