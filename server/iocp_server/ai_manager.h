#pragma once
#include "stdafx.h"

extern std::vector<std::vector<bool>> collision_data;

struct Node {
    int x, y;
    int g; //시작점에서의 비용
    int h; //골까지의 휴리스틱
    char dir; // 노드에 도달하기 위한 방향
    Node* parent;

    Node(int _x, int _y, int _g, int _h, char _dir, Node* _parent = nullptr)
        : x(_x), y(_y), g(_g), h(_h), dir(_dir), parent(_parent) {
    }

    int f() const { return g + h; }

    bool operator>(const Node& other) const {
        return f() > other.f();
    }
};

struct NodeHash {
    size_t operator()(const std::pair<int, int>& p) const {
        return std::hash<int>()(p.first) ^ std::hash<int>()(p.second);
    }
};

void ai_init();

void WakeUp(int npc_id, int waker);

char ai_random_move(int npc_id);

char ai_follow_move(int npc_id, int target_id);

bool keep_alive(int npc_id);

int api_send_hello(lua_State* L);

int api_get_x(lua_State* L);

int api_get_y(lua_State* L);
