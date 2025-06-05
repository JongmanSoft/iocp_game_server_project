#pragma once
#include "Scene.h"

struct input_text {
    TCHAR T_input[20];
    unsigned short count=0;
};

struct s_caret {
    bool showing = false;
    bool select = false;
    POINT pos = { 253,466 };
};

class Start_Scene : public Scene {
public:
   
    CImage title_img;

    FMOD::System* g_system = nullptr;
    FMOD::Sound* g_sound = nullptr;
    FMOD::Channel* g_channel = nullptr;

    RECT IP_input_RT = { 251,463,251 + 209,463 + 31 };
    RECT ID_input_RT = { 251,506,251 + 209,506 + 31 };

    input_text IP_T;
    input_text ID_T;

    s_caret _caret;

public:
    Start_Scene(HWND hwnd, HBITMAP hBufferBitmap, HDC hBufferDC);
    ~Start_Scene(); 
    void update() override;
    void network() override;
    LRESULT CALLBACK windowproc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) override;
public:
    void play_bgm();
};
