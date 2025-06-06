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

using namespace FMOD;
#pragma comment(lib, "gdiplus.lib")