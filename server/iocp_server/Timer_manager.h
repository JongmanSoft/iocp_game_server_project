#pragma once
#include "stdafx.h"

struct TIMER_EVENT {
	int obj_id;
	int wake_time; //주기
	void (*funtion)(const TIMER_EVENT&);
	int target_id;
	constexpr bool operator < (const TIMER_EVENT& L) const
	{
		return (wake_time > L.wake_time);
	}
};
extern concurrency::concurrent_vector <TIMER_EVENT> TIV;
extern concurrency::concurrent_priority_queue<TIMER_EVENT> TIQ; //실행시간이 다된 이벤트만 들어감

void Do_Timer();

void EXEC_EVENT();

void attack_update(TIMER_EVENT&);

void random_move(TIMER_EVENT&);

void follow_move(TIMER_EVENT&);

void attack_player(TIMER_EVENT&);