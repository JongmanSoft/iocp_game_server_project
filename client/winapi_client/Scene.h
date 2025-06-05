#pragma once
#include "stdafx.h"

class Scene
{
public:
	HWND m_hwnd;
	HBITMAP m_hBufferBitmap;
	HDC m_hBufferDC;

	short next_scene;
public:


	Scene() {};
	Scene(HWND hwnd, HBITMAP hBufferBitmap, HDC hBufferDC) {
		m_hwnd = hwnd;
		m_hBufferBitmap = hBufferBitmap;
		m_hBufferDC = hBufferDC;
	};

public:
	bool running = 0;


public:
	//virtualÇÔ¼öµé

	virtual void update() = 0;

	virtual void network() = 0;

	virtual LRESULT CALLBACK windowproc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}

};

