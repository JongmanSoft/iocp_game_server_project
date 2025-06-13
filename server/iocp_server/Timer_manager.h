#pragma once
#include "stdafx.h"

enum EVENT_TYPE { EV_RANDOM_MOVE, EV_RUN_AWAY };

struct TIMER_EVENT {
	int obj_id;
	chrono::system_clock::time_point wakeup_time;
	EVENT_TYPE event_id;
	int target_id;
	constexpr bool operator < (const TIMER_EVENT& L) const
	{
		return (wakeup_time > L.wakeup_time);
	}
};

extern concurrency::concurrent_priority_queue<TIMER_EVENT> timer_queue;

void timer_thread();
