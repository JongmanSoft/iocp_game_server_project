#include "Timer_manager.h"
#include "network_manager.h"
concurrency::concurrent_priority_queue<TIMER_EVENT> TIQ;

void Do_Timer()
{
    while (true) {
        // 0.5√  ¥Î±‚
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
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
                event.wake_time -= 0.5;
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
    ov->_comp_type = OP_OBJECT_RELIVE;
    PostQueuedCompletionStatus(h_iocp, 1, ev.obj_id, &ov->_over);
}

void random_move(const TIMER_EVENT& ev)
{
    OVER_EXP* ov = new OVER_EXP;
    ov->_comp_type = OP_NPC_RANDOM_MOVE;
    PostQueuedCompletionStatus(h_iocp, 1, ev.obj_id, &ov->_over);
}

void follow_move(const TIMER_EVENT& ev)
{
    OVER_EXP* ov = new OVER_EXP;
    ov->_comp_type = OP_NPC_FOLLOW;
    ov->_ai_target_obj = ev.target_id;
    PostQueuedCompletionStatus(h_iocp, 1, ev.obj_id, &ov->_over);
}




