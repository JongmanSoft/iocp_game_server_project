#include "Timer_manager.h"

concurrency::concurrent_priority_queue<TIMER_EVENT> TIQ;

void Do_Timer()
{
    while (true) {
        // 1초 대기
        std::this_thread::sleep_for(std::chrono::seconds(1));
        if (TIQ.empty()) {
            continue;
        }
        std::vector<TIMER_EVENT> events;
        TIMER_EVENT ev;
        while (TIQ.try_pop(ev)) {
            events.push_back(ev);
        }
        for (auto& event : events) {
            if (event.wake_time <= 0) {
                event.funtion(event);
            }
            else {
                event.wake_time -= 1;
                TIQ.push(event);
            }
        }
    }
}


void attack_update(const TIMER_EVENT&)
{
    std::cout << "나 실행!" << std::endl;
}

void random_move(const TIMER_EVENT&)
{
}

void follow_move(const TIMER_EVENT&)
{
}

void attack_player(const TIMER_EVENT&)
{
}


