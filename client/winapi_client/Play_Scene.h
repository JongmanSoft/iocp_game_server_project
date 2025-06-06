#pragma once
#include "Scene.h"
class Play_Scene :
	public Scene
{
public:
     Play_Scene(HWND hwnd, HBITMAP hBufferBitmap, HDC hBufferDC, void* fw);
    ~Play_Scene();
    void update() override;
    void network() override;
    LRESULT CALLBACK windowproc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) override;
};

