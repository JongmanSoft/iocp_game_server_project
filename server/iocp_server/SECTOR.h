#pragma once
#include <concurrent_unordered_set.h>
#include <mutex>
#include <array>
#include "protocol.h"

constexpr int VIEW_RANGE = 7;
constexpr int SECTOR_SIZE = 20; //20X20사이즈의 섹터
extern std::array<std::array<concurrency::concurrent_unordered_set<int>, (MAP_HEIGHT / SECTOR_SIZE) + 1>, (MAP_WIDTH / SECTOR_SIZE) + 1> g_sectors;
extern std::array<std::array<std::mutex, MAP_WIDTH / SECTOR_SIZE>, MAP_HEIGHT / SECTOR_SIZE> g_sector_mutexes;

std::pair<int, int>get_sector_index(const int& new_x, const int& new_y);

std::pair<int, int> insert_sector(const int& c_id, const int& new_x, const int& new_y);

std::pair<int, int> delete_sector(const int& c_id, const int& _x, const int& _y);

bool can_see(int from, int to);

bool is_pc(int object_id);

bool is_npc(int object_id);