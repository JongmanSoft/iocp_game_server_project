#pragma once
#include "stdafx.h"

class TIMER_EVENT {
public:
	int obj_id;
	int wake_time; //�ֱ�,�� ����
	void (*funtion)(const TIMER_EVENT&);
	int target_id;
	constexpr bool operator < (const TIMER_EVENT& L) const
	{
		return (wake_time > L.wake_time);
	}
public:
	TIMER_EVENT() {};
	TIMER_EVENT(int objID, int wakeTime, void(*func)(const TIMER_EVENT&), int target) : obj_id{ objID },
		wake_time{ wakeTime }, funtion{ func }, target_id{ target } {
	};
};


extern concurrency::concurrent_priority_queue<TIMER_EVENT> TIQ; //����ð��� �ٵ� �̺�Ʈ�� ��

void Do_Timer();

void attack_update(const TIMER_EVENT&);

void random_move(const TIMER_EVENT&);

void follow_move(const TIMER_EVENT&);

void attack_player(const TIMER_EVENT&);