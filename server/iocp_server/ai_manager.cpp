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
        OVER_EXP* exover = new OVER_EXP;
        exover->_comp_type = OP_PLAYER_DAMMAGE;
        exover->_ai_target_obj = npc_id; //해를 가한 npc의 아이디
        PostQueuedCompletionStatus(h_iocp, 1, waker, &exover->_over);
       
       }
        break;
    case S_HUMAN:
        {
        OVER_EXP* exover = new OVER_EXP;
        exover->_comp_type = OP_PLAYER_DAMMAGE;
        exover->_ai_target_obj = npc_id;  //해를 가한 npc의 아이디
        PostQueuedCompletionStatus(h_iocp, 1, waker, &exover->_over);
              }
        break;
    default:
        break;
    }
   
}

char ai_random_move(int npc_id)
{
    //방향을 리턴
    auto it = object.find(npc_id);
    if (it == object.end())return MOVE_DOWN;
    shared_ptr<NPC>npc = std::dynamic_pointer_cast<NPC>(it->second.load());
    char random_move_dir = rand() % 4 + 1;
    switch (random_move_dir)
    {
    case MOVE_UP: //LEFT
        if (npc->x > 0 && npc->x - 1 > npc->sx - 10) {
            short find_index_x = npc->x % 100;
            if (find_index_x == 0) find_index_x = 100;
            if (!collision_data[find_index_x - 1][npc->y%100])npc->x--;
        }
        break;
    case MOVE_DOWN: //DOWN
        if (npc->y < MAP_HEIGHT && npc->y + 1 < npc->sy + 10) {
            short find_index_y = npc->y % 100;
            if (find_index_y == 99) find_index_y = 0;
            if (!collision_data[npc->x % 100][find_index_y + 1])npc->y++;
        }
        break;
    case MOVE_LEFT: //RIGHT
        if (npc->x < MAP_WIDTH && npc->x + 1 < npc->sx + 10) {
            short find_index_x = npc->x % 100;
            if (find_index_x == 99) find_index_x = 0;
            if (!collision_data[find_index_x + 1][npc->y % 100])npc->x++;
        }
        break;
    case MOVE_RIGHT: //UP
        if (npc->y > 0 && npc->y - 1 > npc->sy - 10) {
            short find_index_y = npc->y % 100;
            if (find_index_y == 0) find_index_y = 100;
            if (!collision_data[npc->x%100][find_index_y - 1])npc->y--;
        }
        break;
    default:
        break;
    }
    return random_move_dir;
}

char ai_follow_move(int npc_id, int target_id)
{
    //방향을 리턴
}

bool keep_alive(int npc_id)
{
    auto it = object.find(npc_id);
    if (it == object.end()) return false;
    std::shared_ptr<OBJECT> c = it->second.load();
    std::pair<int, int>index = get_sector_index(c->x, c->y);
    std::pair<int, int> min_index = { max(0,index.first - 1), max(0,index.second - 1) };
    std::pair<int, int> max_index = {
        min(MAP_HEIGHT / SECTOR_SIZE - 1,index.first + 1),
            min(MAP_WIDTH / SECTOR_SIZE - 1,index.second + 1) };

    for (int row = min_index.first; row < max_index.first + 1; row++) {
        for (int col = min_index.second; col < max_index.second + 1; col++) {
            concurrency::concurrent_unordered_set<int> local_list;
            {
                std::lock_guard<std::mutex> ll(g_sector_mutexes[row][col]);
                local_list = g_sectors[row][col];
            }

            for (const int& p_id : local_list) {
                shared_ptr<OBJECT>p = object.at(p_id);
                {
                    lock_guard<mutex> ll(p->_s_lock);
                    if (ST_INGAME != p->_state) continue;
                }
                if (p->_id == npc_id) continue;
                if (false == can_see(npc_id, p->_id))
                    continue;
                if (is_pc(p->_id)) {
                    shared_ptr <USER> PP = std::dynamic_pointer_cast<USER>(p);
                    return true;
                }

            }
        }
    }

    return false;
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


