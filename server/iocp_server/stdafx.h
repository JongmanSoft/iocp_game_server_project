#pragma once
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>
#include <MSWSock.h>
#include <windows.h>
#include <iostream>
#include <array>
#include <string.h>
#include <thread>
#include <vector>
#include <memory>
#include <mutex>
#include <unordered_set>
#include <concurrent_unordered_map.h>
#include <concurrent_unordered_set.h>
#include <concurrent_priority_queue.h>
#include <concurrent_vector.h>
#include <concurrent_queue.h>
#include <sqlext.h>
#include <locale.h>
#include "protocol.h"
#include "include/lua.hpp"

#pragma comment(lib, "WS2_32.lib")
#pragma comment(lib, "MSWSock.lib")
#pragma comment(lib, "lua54.lib")
using namespace std;

bool hasSpecialChar(const std::string& str);