#pragma once
#include <winsock2.h>
#include <MSWSock.h>
#include "protocol.h"

struct USER_TABLE {
    char uid[20];
    int x, y;
    int level;
    int hp;
};

enum COMP_TYPE { OP_ACCEPT, OP_RECV, OP_SEND, 
    OP_PLAYER_ATTACK, OP_PLAYER_SKILL, OP_PLAYER_HEAL, OP_PLAYER_HEAL_SKILL,OP_PLAYER_DAMMAGE,
    OP_NPC_MOVE, OP_AI_HELLO, OP_NPC_RUN_AWAY, 
    OP_DB_NEW_USER, OP_DB_LOAD_USER };

class OVER_EXP {
public:
    WSAOVERLAPPED _over;
    WSABUF _wsabuf;
    char _send_buf[BUF_SIZE];
    COMP_TYPE _comp_type;
    USER_TABLE result_info;
    int _ai_target_obj;

    OVER_EXP() {
        _wsabuf.len = BUF_SIZE;
        _wsabuf.buf = _send_buf;
        _comp_type = OP_RECV;
        ZeroMemory(&_over, sizeof(_over));
    }

    OVER_EXP(char* packet) {
        _wsabuf.len = packet[0];
        _wsabuf.buf = _send_buf;
        ZeroMemory(&_over, sizeof(_over));
        _comp_type = OP_SEND;
        memcpy(_send_buf, packet, packet[0]);
    }
};