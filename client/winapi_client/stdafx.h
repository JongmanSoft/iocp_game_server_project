#pragma once
#include <winsock2.h> 
#include <windows.h>
#include <ws2tcpip.h>
#include <iostream>
#include <array>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <chrono>
#include <thread>
#include <memory>
#include <queue>
#include <functional>
#include <unordered_map>
#include <mutex>
#include <atlimage.h>
#include "fmod.hpp"
#include "fmod_errors.h"


#include "../../server/iocp_server/protocol.h"
#define window_size 640
#define START_SCENE 0
#define PLAY_SCENE 1
#define MAP_SIZE MAP_HEIGHT
#define MAP_CULLING 20

using namespace FMOD;
#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "Ws2_32.lib")
