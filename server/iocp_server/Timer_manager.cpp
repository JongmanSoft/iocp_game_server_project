#include "Timer_manager.h"
#include "network_manager.h"
concurrency::concurrent_priority_queue<TIMER_EVENT> TIQ;

void Do_Timer()
{
    while (true) {
        // 1√  ¥Î±‚
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


void attack_update(const TIMER_EVENT& ev)
{
    OVER_EXP* ov = new OVER_EXP;
    ov->_comp_type = OP_PLAYER_ATTACK;
    PostQueuedCompletionStatus(h_iocp, 1, ev.obj_id, &ov->_over);
}

void attack_skill_update(const TIMER_EVENT& ev)
{
    OVER_EXP* ov = new OVER_EXP;
    ov->_comp_type = OP_PLAYER_SKILL;
    PostQueuedCompletionStatus(h_iocp, 1, ev.obj_id, &ov->_over);
}

void heal_skiil_update(const TIMER_EVENT& ev)
{
    OVER_EXP* ov = new OVER_EXP;
    ov->_comp_type = OP_PLAYER_HEAL_SKILL;
    PostQueuedCompletionStatus(h_iocp, 1, ev.obj_id, &ov->_over);
}

void heal_update(const TIMER_EVENT& ev)
{
    OVER_EXP* ov = new OVER_EXP;
    ov->_comp_type = OP_PLAYER_HEAL;
    PostQueuedCompletionStatus(h_iocp, 1, ev.obj_id, &ov->_over);
}

void relive_update(const TIMER_EVENT& ev)
{
    OVER_EXP* ov = new OVER_EXP;
    ov->_comp_type = OP_PLAYER_RELIVE;
    PostQueuedCompletionStatus(h_iocp, 1, ev.obj_id, &ov->_over);
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


