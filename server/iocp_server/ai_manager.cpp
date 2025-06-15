#include "ai_manager.h"
#include "network_manager.h"
#include "global_data.h"

std::vector<std::vector<bool>> collision_data;

void ai_init()
{
    collision_data.resize(100, std::vector<bool>(100));
    std::ifstream file("collision_data_small.csv");
    std::vector<std::vector<std::string>> data;
    std::string line;

    while (std::getline(file, line)) {
        std::vector<std::string> row;
        std::stringstream ss(line);
        std::string cell;
        while (std::getline(ss, cell, ',')) {
            row.push_back(cell);
        }
        data.push_back(row);
    }
    file.close();

    for (size_t i = 0; i < 100; ++i) {
        for (size_t j = 0; j < 100; ++j) {
            if (i < data.size() && j < data[i].size()) {
                try {
                    int value = std::stoi(data[i][j]);
                    collision_data[j][i] = value;
             
                }
                catch (const std::exception& e) {
                    std::cerr << "데이터 변환 오류: (" << i << ", " << j << ")" << std::endl;
                }
            }
        }
    }
    
}

void WakeUp(int npc_id, int waker)
{
    auto it = object.find(npc_id);
    if (it == object.end())return ;
    shared_ptr<OBJECT>c = it->second.load();
    switch (c->_o_type)
    {
    case ORC_NPC:
    {
        OVER_EXP* exover = new OVER_EXP;
        exover->_comp_type = OP_AI_HELLO;
        exover->_ai_target_obj = waker;
        PostQueuedCompletionStatus(h_iocp, 1, npc_id, &exover->_over);
    }
       break;
    case HUMAN:
        {
       
       
       }
        break;
    case S_HUMAN:
        {

              }
        break;
    default:
        break;
    }
   
}

int api_send_hello(lua_State* L)
{
    int my_id = (int)lua_tointeger(L, -3);
    int user_id = (int)lua_tointeger(L, -2);
    char* mess = (char*)lua_tostring(L, -1);
    lua_pop(L, 4);
    auto it = object.find(user_id);
    if (it == object.end())return 0;
    shared_ptr<USER>c = std::dynamic_pointer_cast<USER>(it->second.load());
    c->send_chat_packet(my_id, mess);
    return 1;
}

int api_get_x(lua_State* L)
{

    int user_id =
        (int)lua_tointeger(L, -1);
    lua_pop(L, 2);
    auto it = object.find(user_id);
    if (it == object.end())return 0;
    shared_ptr<OBJECT>c = it->second.load();
    int x = c->x;
    lua_pushnumber(L, x);
    return 1;
}

int api_get_y(lua_State* L)
{
    int user_id =
        (int)lua_tointeger(L, -1);
    lua_pop(L, 2);
    auto it = object.find(user_id);
    if (it == object.end())return 0;
    shared_ptr<OBJECT>c = it->second.load();
    int y = c->y;
    lua_pushnumber(L, y);
    return 1;
}


