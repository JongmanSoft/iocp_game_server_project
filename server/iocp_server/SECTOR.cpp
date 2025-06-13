#include "stdafx.h"
#include "SECTOR.h"


std::array<std::array<concurrency::concurrent_unordered_set<int>, (MAP_HEIGHT / SECTOR_SIZE) + 1>, (MAP_WIDTH / SECTOR_SIZE) + 1> g_sectors;
std::array<std::array<std::mutex, MAP_WIDTH / SECTOR_SIZE>, MAP_HEIGHT / SECTOR_SIZE> g_sector_mutexes;

std::pair<int, int> get_sector_index(const int& new_x, const int& new_y)
{
	int sector_index_x = new_y / SECTOR_SIZE;
	int sector_index_y = new_x / SECTOR_SIZE;
	return { sector_index_x,sector_index_y };
}

std::pair<int, int> insert_sector(const int& c_id, const int& new_x, const int& new_y)
{
	auto index = get_sector_index(new_x, new_y);
	g_sectors[index.first][index.second].insert(c_id);
	return index;
}

std::pair<int, int> delete_sector(const int& c_id, const int& _x, const int& _y)
{
	auto index = get_sector_index(_x, _y);
	std::lock_guard<std::mutex> lock(g_sector_mutexes[index.first][index.second]);
	g_sectors[index.first][index.second].unsafe_erase(c_id);
	return index;
}

bool can_see(int from, int to)
{
	//투디게임의 시야는 원형이 아니므로 사각형 범위로 계산
	shared_ptr<OBJECT>from_p = object.at(from);
	shared_ptr<OBJECT>to_p = object.at(to);
	return (abs(from_p->x - to_p->x) <= VIEW_RANGE) && abs(from_p->y - to_p->y) <= VIEW_RANGE;
}