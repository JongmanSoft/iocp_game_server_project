#pragma once
#include "stdafx.h"

class TIMER_EVENT {
public:
	int obj_id;
	float wake_time; //주기,초 단위
	void (*funtion)(const TIMER_EVENT&);
	int target_id;
	constexpr bool operator < (const TIMER_EVENT& L) const
	{
		return (wake_time > L.wake_time);
	}
public:
	TIMER_EVENT() {};
	TIMER_EVENT(int objID, float wakeTime, void(*func)(const TIMER_EVENT&), int target) : obj_id{ objID },
		wake_time{ wakeTime }, funtion{ func }, target_id{ target } {
	};
};


extern concurrency::concurrent_priority_queue<TIMER_EVENT> TIQ; //실행시간이 다된 이벤트만 들어감

void Do_Timer();
//플레이어 주기 (공격,범위스킬,회복스킬,회복)
void attack_update(const TIMER_EVENT&);
void attack_skill_update(const TIMER_EVENT&);
void heal_skiil_update(const TIMER_EVENT&);
void heal_update(const TIMER_EVENT&);
void relive_update(const TIMER_EVENT&);
void random_move(const TIMER_EVENT&);
void follow_move(const TIMER_EVENT&);

