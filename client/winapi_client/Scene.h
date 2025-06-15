#pragma once
#include "stdafx.h"

class Scene
{
public:
	HWND m_hwnd;
	HBITMAP m_hBufferBitmap;
	HDC m_hBufferDC;
	HINSTANCE m_hinstance;
	void* m_fw;
public:


	Scene() {};
	Scene(HWND hwnd, HBITMAP hBufferBitmap, HDC hBufferDC,HINSTANCE m_hinstance) {
		m_hwnd = hwnd;
		m_hBufferBitmap = hBufferBitmap;
		m_hBufferDC = hBufferDC;
	};
	Scene(HWND hwnd, HBITMAP hBufferBitmap, HDC hBufferDC, HINSTANCE m_hinstance,void* fw) {
		m_hwnd = hwnd;
		m_hBufferBitmap = hBufferBitmap;
		m_hBufferDC = hBufferDC;
		m_fw = fw;
	};

public:
	bool running = 0;


public:
	//virtualÇÔ¼öµé

	virtual void update() = 0;

	virtual void network() = 0;

	virtual void shutdown() = 0;

	virtual LRESULT CALLBACK windowproc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}

	void scene_change(int next_scene);

};

