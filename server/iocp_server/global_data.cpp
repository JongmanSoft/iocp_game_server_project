#include "stdafx.h"
#include "global_data.h"

concurrency::concurrent_unordered_map<long long, std::atomic<std::shared_ptr<OBJECT>>> object;

int get_max_hp(int level)
{
    return 0;
}

int get_attack_damage(int level)
{
    return 0;
}

int get_need_exp(int level)
{
    return 0;
}
