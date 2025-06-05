#include "stdafx.h"
#include "Framework.h"

Framework::~Framework()
{
	if (m_scene) delete m_scene;
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

LRESULT Framework::windowproc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (m_scene != nullptr) m_scene->windowproc(hwnd, uMsg, wParam, lParam);
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}