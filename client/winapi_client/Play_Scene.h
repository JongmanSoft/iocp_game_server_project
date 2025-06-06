#pragma once
#include "Scene.h"
#include "Map_data.h"
class Play_Scene :
	public Scene
{
public:
    CImage tileset_img;

    FMOD::System* g_system = nullptr;
    FMOD::Sound* g_sound = nullptr;
    FMOD::Channel* g_channel = nullptr;

    POINT camera_pos = { 1979,1979 };
public:
     Play_Scene(HWND hwnd, HBITMAP hBufferBitmap, HDC hBufferDC, void* fw);
    ~Play_Scene();
    void update() override;
    void network() override;
    void shutdown()override;
    LRESULT CALLBACK windowproc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) override;
};

