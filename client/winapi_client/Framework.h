#pragma once
#include "Scene.h"
#include "Start_Scene.h"
#include "Play_Scene.h"



struct login_info
{
	char name[MAX_ID_LENGTH];
	long long  id;
	short x, y;
	short max_hp;
	short hp;
	short level;
	int   exp;
};



class Framework
{

public:
	std::unique_ptr<Scene> m_scene;
	HWND m_hwnd;
	HBITMAP m_hBufferBitmap;
	HDC m_hBufferDC;
	login_info player_login_info;
public:
	Framework(HWND hwnd, HBITMAP hBufferBitmap, HDC hBufferDC);
	~Framework();
public:

	void update();

	void network();

	void scene_change(int next_scene);

	LRESULT CALLBACK windowproc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	void move_packet_process(sc_packet_move);
	void add_packet_process(sc_packet_enter);
	void leave_packet_process(sc_packet_leave);
	void stat_change_packet_process(sc_packet_stat_change);
	void state_packet_process(sc_packet_state);
};

