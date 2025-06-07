#pragma once
#include <windows.h>
#include <iostream>
#include <array>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <chrono>
#include <thread>
#include <atlimage.h>
#include "fmod.hpp"
#include "fmod_errors.h"
#define window_size 640
#define START_SCENE 0
#define PLAY_SCENE 1
#define MAP_SIZE 2000
#define MAP_CULLING 20

using namespace FMOD;
#pragma comment(lib, "gdiplus.lib")

//type, state
constexpr int PLAYER = 0;
constexpr int ORC_NPC = 1;
constexpr int HUMAN = 2;
constexpr int S_HUMAN = 3;

constexpr int IDLE = 0;
constexpr int WALK = 1;
constexpr int HURT = 2;
constexpr int ATTACK = 3;
constexpr int DEATH = 4;