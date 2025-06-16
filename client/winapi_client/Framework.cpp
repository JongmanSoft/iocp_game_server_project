#include "stdafx.h"
#include "Framework.h"
#include "network_data.h"



Framework::Framework(HWND hwnd, HBITMAP hBufferBitmap, HDC hBufferDC,HINSTANCE hInstance)
{
	m_hwnd = hwnd;
	m_hBufferBitmap = hBufferBitmap;
	m_hBufferDC = hBufferDC;
	m_hinstance = hInstance;
	m_scene = std::make_unique<Start_Scene>(m_hwnd, m_hBufferBitmap, m_hBufferDC,hInstance, this);

	NonBlockingClient::get_inst().frame_work_recv(this);
}

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
	m_scene->shutdown();
	switch (next_scene)
	{
	case START_SCENE:
	{
		m_scene = std::make_unique<Start_Scene>(m_hwnd, m_hBufferBitmap, m_hBufferDC,m_hinstance,this);
	}
	break;
	case PLAY_SCENE:
	{
		m_scene = std::make_unique<Play_Scene>(m_hwnd, m_hBufferBitmap, m_hBufferDC, m_hinstance,this);
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

void Framework::move_packet_process(sc_packet_move p)
{
	Play_Scene* scene = static_cast<Play_Scene*>(m_scene.get());
	if (p.id == player_login_info.id) {
		player_login_info.x = p.x;
		player_login_info.y = p.y;
		scene->player->_x = p.x;
		scene->player->_y = p.y;
		return;
	}
	auto c = scene->objects.find(p.id);
	if (c == scene->objects.end()) return;
	std::shared_ptr <object> obj = c->second.load();
	obj->_x = p.x;
	obj->_y = p.y;

}

void Framework::add_packet_process(sc_packet_enter p)
{
	Play_Scene* scene = static_cast<Play_Scene*>(m_scene.get());
	
	std::shared_ptr<object> obj = std::make_shared<object>(p.o_type, p.x, p.y, p.name);
	scene->objects.insert(std::make_pair(p.id, obj));
	
}

void Framework::leave_packet_process(sc_packet_leave p )
{
	Play_Scene* scene = static_cast<Play_Scene*>(m_scene.get());
	scene->objects.erase(p.id);
}

void Framework::stat_change_packet_process(sc_packet_stat_change p)
{
	Play_Scene* scene = static_cast<Play_Scene*>(m_scene.get());
	if (p.id == player_login_info.id) {
		scene->player->_hp = p.hp;
		scene->player->_level = p.level;
		scene->player->_exp = p.exp;
		return;
	}
	auto c = scene->objects.find(p.id);
	if (c == scene->objects.end()) return;
	std::shared_ptr <object> obj = c->second.load();
	obj->_hp = p.hp;
	obj->_level = p.level;
	obj->_exp = p.exp;
}

void Framework::state_packet_process(sc_packet_state p)
{
	Play_Scene* scene = static_cast<Play_Scene*>(m_scene.get());
	if (p.id == player_login_info.id) {
		scene->change_state(p.state);
		return;
	}
	auto c = scene->objects.find(p.id);
	if (c == scene->objects.end()) return;
	std::shared_ptr <object> obj = c->second.load();
	int client_dir[4] = {1,0,2,3};
	obj->change_state(p.state, client_dir[p.direction - 1]);
}

void Framework::chat_packet_process(sc_packet_chat p)
{
	Play_Scene* scene = static_cast<Play_Scene*>(m_scene.get());

	if (player_login_info.id == p.id) {
		scene->player->mess_ptr->mess_setting(p.message);
		return;
	}
	auto c = scene->objects.find(p.id);
	if (c == scene->objects.end()) return;
	std::shared_ptr <object> obj = c->second.load();
	obj->mess_ptr->mess_setting(p.message);
}
