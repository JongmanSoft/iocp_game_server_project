#include "object.h"

void object::draw(HDC m_hBufferDC)
{
	state_ptr->render(m_hBufferDC);
	//이름출력

	HFONT hFont = CreateFont(12, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_DONTCARE, L"둥근모꼴");
	SelectObject(m_hBufferDC, hFont);
	SetTextColor(m_hBufferDC, RGB(90, 37, 23));
	std::string out_name = _name;
	int wlen = MultiByteToWideChar(CP_ACP, 0, out_name.c_str(), -1, NULL, 0);
	std::wstring name_wstr(wlen - 1, L'\0');
	MultiByteToWideChar(CP_ACP, 0, out_name.c_str(), -1, &name_wstr[0], wlen);
	int render_x = MAP_CULLING / 2;
	int render_y = MAP_CULLING / 2;
	TextOut(m_hBufferDC, (render_x * 32) -10, (render_y * 32) - 16, name_wstr.c_str(), name_wstr.length());

	mess_ptr->render(m_hBufferDC);
	DeleteObject(hFont);
}

void object::draw(HDC m_hBufferDC, int px, int py)
{
	state_ptr->render(m_hBufferDC,_x,_y,px,py);
	//이름출력
	HFONT hFont = CreateFont(12, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_DONTCARE, L"둥근모꼴");
	SelectObject(m_hBufferDC, hFont);
	SetTextColor(m_hBufferDC, RGB(90, 37, 23));
	std::string out_name = _name;
	int wlen = MultiByteToWideChar(CP_ACP, 0, out_name.c_str(), -1, NULL, 0);
	std::wstring name_wstr(wlen - 1, L'\0');
	MultiByteToWideChar(CP_ACP, 0, out_name.c_str(), -1, &name_wstr[0], wlen);
	int render_x = _x - (px - (MAP_CULLING / 2));
	int render_y = _y - (py - (MAP_CULLING / 2));
	TextOut(m_hBufferDC, (render_x * 32) -10, (render_y * 32) - 16, name_wstr.c_str(), name_wstr.length());

	mess_ptr->render(m_hBufferDC, _x, _y, px, py);
	DeleteObject(hFont);
}

void object::update()
{
	state_ptr->update();
	mess_ptr->update();
}

void object::change_state(int state_type, int dir)
{
	int curr_frame = state_ptr->current_frame;
	switch (state_type)
	{
	case IDLE:
		state_ptr = std::make_unique<loop_state>(_type, state_type, this);
		break;
	case WALK:
		state_ptr = std::make_unique<return_state>(_type, state_type, this);
		state_ptr->current_frame = curr_frame;
		break;
	case ATTACK:
		state_ptr = std::make_unique<return_state>(_type, state_type, this);
		break;
	case HURT:
		state_ptr = std::make_unique<return_state>(_type, state_type, this);
		break;
	case DEATH:
		state_ptr = std::make_unique<end_state>(_type, state_type, this);
		break;
	default:
		break;
	}
	state_ptr->dir = dir;
}
