#pragma once
#include <concurrent_unordered_map.h>
#include <atomic>
#include "OBJECT.h"
#include <memory> 
class OBJECT;
extern concurrency::concurrent_unordered_map<long long, std::atomic<std::shared_ptr<OBJECT>>> object;