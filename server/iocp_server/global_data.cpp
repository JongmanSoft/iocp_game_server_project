#include "stdafx.h"
#include "global_data.h"

concurrency::concurrent_unordered_map<long long, std::atomic<std::shared_ptr<OBJECT>>> object;