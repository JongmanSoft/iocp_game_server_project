#pragma once
#include "Scene.h"
#include "Start_Scene.h"
#include "Play_Scene.h"


class Framework
{

public:
	std::unique_ptr<Scene> m_scene;
	HWND m_hwnd;
	HBITMAP m_hBufferBitmap;
	HDC m_hBufferDC;

public:
	Framework(HWND hwnd, HBITMAP hBufferBitmap, HDC hBufferDC);
	~Framework();
public:

	void update();

	void network();

	void scene_change(int next_scene);

	LRESULT CALLBACK windowproc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

};

