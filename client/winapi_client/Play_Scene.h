#pragma once
#include "Scene.h"
#include "Map_data.h"
#include "object.h"
class Play_Scene :
	public Scene
{
public:
    CImage tileset_img;

    FMOD::System* g_system = nullptr;
    FMOD::Sound* g_sound = nullptr;
    FMOD::Channel* g_channel = nullptr;

    POINT camera_pos = { 1000,1000 };

    std::unique_ptr<object> player;
public:
     Play_Scene(HWND hwnd, HBITMAP hBufferBitmap, HDC hBufferDC, void* fw);
    ~Play_Scene();
    void update() override;
    void network() override;
    void shutdown()override;
    LRESULT CALLBACK windowproc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) override;
};

