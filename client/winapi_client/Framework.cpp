#include "stdafx.h"
#include "Framework.h"

Framework::~Framework()
{
}


void Framework::update()
{
	if (m_scene != nullptr)m_scene->update();

}

void Framework::network()
{
	if (m_scene != nullptr)
		m_scene->network();
}

void Framework::scene_change(int next_scene)
{
	switch (next_scene)
	{
	case START_SCENE:
	{
		m_scene = std::make_unique<Start_Scene>(m_hwnd, m_hBufferBitmap, m_hBufferDC,this);
	}
	break;
	case PLAY_SCENE:
	{
		m_scene = std::make_unique<Play_Scene>(m_hwnd, m_hBufferBitmap, m_hBufferDC,this);
	}
	break;
	default:
		break;
	}
}

LRESULT Framework::windowproc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (m_scene != nullptr) m_scene->windowproc(hwnd, uMsg, wParam, lParam);
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}