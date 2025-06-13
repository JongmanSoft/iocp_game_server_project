#pragma once
#include <iostream>
#include <array>
#include <WS2tcpip.h>
#include <MSWSock.h>
#include <string.h>
#include <thread>
#include <vector>
#include <memory>
#include <mutex>
#include <unordered_set>
#include <concurrent_unordered_map.h>
#include <concurrent_unordered_set.h>
#include <concurrent_priority_queue.h>
#include <concurrent_queue.h>
#include <sqlext.h>  
#include <locale.h>
#include "protocol.h"
#include "OBJECT.h"
#include "include/lua.hpp"

#pragma comment(lib, "WS2_32.lib")
#pragma comment(lib, "MSWSock.lib")
#pragma comment(lib, "lua54.lib")
using namespace std;

extern concurrency::concurrent_unordered_map<long long,
	std::atomic<std::shared_ptr<OBJECT>>>object;

//DB접근 결과를 알리기 위한 유저정보 확장 오버랩드 구조체에 넣을것.
struct USER_TABLE 
{
	char uid[20];
	int x, y;
	int level;
	int hp;
};
