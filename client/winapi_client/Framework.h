#pragma once
#include "Scene.h"
#include "Start_Scene.h"
class Framework
{

public:
	Scene* m_scene;
	HWND m_hwnd;
	HBITMAP m_hBufferBitmap;
	HDC m_hBufferDC;

	short current_scene = START_SCENE;

public:
	Framework(HWND hwnd, HBITMAP hBufferBitmap, HDC hBufferDC) {
		m_hwnd = hwnd;
		m_hBufferBitmap = hBufferBitmap;
		m_hBufferDC = hBufferDC;
		m_scene = new Start_Scene(m_hwnd, m_hBufferBitmap, m_hBufferDC);
		m_scene->next_scene = START_SCENE;
	}
	~Framework();
public:

	void update();

	void network();

	LRESULT CALLBACK windowproc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

};

