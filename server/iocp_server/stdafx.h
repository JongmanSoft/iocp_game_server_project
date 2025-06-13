#pragma once
#include <iostream>
#include <array>
#include <WS2tcpip.h>
#include <MSWSock.h>
#include <string.h>
#include <thread>
#include <vector>
#include <mutex>
#include <unordered_set>
#include <concurrent_unordered_map.h>
#include <concurrent_unordered_set.h>
#include <concurrent_priority_queue.h>
#include <sqlext.h>  
#include <locale.h>
#include "protocol.h"

#include "include/lua.hpp"

#pragma comment(lib, "WS2_32.lib")
#pragma comment(lib, "MSWSock.lib")
#pragma comment(lib, "lua54.lib")
using namespace std;