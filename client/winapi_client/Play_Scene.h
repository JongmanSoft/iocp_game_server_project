#pragma once
#include "Scene.h"
#include "Map_data.h"
#include "object.h"
class Play_Scene :
	public Scene
{
public:
    CImage tileset_img;
    CImage profile_img;
    CImage hp_img;

    FMOD::System* g_system = nullptr;
    FMOD::Sound* g_bgm = nullptr;
    FMOD::Sound* g_attack = nullptr;
    FMOD::Sound* g_hurt = nullptr;
    FMOD::Sound* g_death = nullptr;
    FMOD::Channel* g_channel = nullptr;

    POINT camera_pos = { 1040,1028 };

    std::unique_ptr<object> player;
public:
    HWND hEdit;
public:
     Play_Scene(HWND hwnd, HBITMAP hBufferBitmap, HDC hBufferDC, void* fw);
    ~Play_Scene();
    void update() override;
    void network() override;
    void shutdown()override;
    LRESULT CALLBACK windowproc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) override;
public:
    //네트워크
    void update_chat_log(TCHAR* chat);
    void change_state(int state);
};

