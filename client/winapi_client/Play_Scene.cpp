#include "Play_Scene.h"


Play_Scene::Play_Scene(HWND hwnd, HBITMAP hBufferBitmap, HDC hBufferDC, void* fw)
{
	m_hwnd = hwnd;
	m_hBufferBitmap = hBufferBitmap;
	m_hBufferDC = hBufferDC;
    m_fw = fw;
    tileset_img.Load(L"image/tileset.png");

    player = std::make_unique<object>(PLAYER, camera_pos.x+MAP_CULLING/2, camera_pos.y+ MAP_CULLING / 2);
   
    //sound
    FMOD_RESULT result = FMOD::System_Create(&g_system);
    if (result == FMOD_OK) {
        result = g_system->init(32, FMOD_INIT_NORMAL, nullptr);
        if (result == FMOD_OK) {
            result = g_system->createSound("sound/BGM_play.mp3", FMOD_DEFAULT | FMOD_LOOP_NORMAL, nullptr, &g_sound);
            if (result == FMOD_OK) {
                g_system->playSound(g_sound, nullptr, false, &g_channel);
            }
        }
    }
}

Play_Scene::~Play_Scene()
{

}

void Play_Scene::update()
{
    player->state_ptr->update();
}

void Play_Scene::network()
{
}

void Play_Scene::shutdown()
{
    if (g_channel != nullptr) {
        g_channel->stop();
    }
    if (g_sound != nullptr) {
        g_sound->release();
        g_sound = nullptr;
    }
    if (g_system != nullptr) {
        g_system->close();
        g_system->release();
        g_system = nullptr;
    }
}




LRESULT Play_Scene::windowproc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg) {
    case WM_CREATE:
    {
        return 0;
    }
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        RECT rect;
        GetClientRect(m_hwnd, &rect);

        // 배경
        FillRect(m_hBufferDC, &rect, (HBRUSH)GetStockObject(BLACK_BRUSH));

        //
        SetBkMode(m_hBufferDC, TRANSPARENT);
        SetTextColor(m_hBufferDC, RGB(255, 255, 255));

        for (int a = 0; a < 20; a++) {
            if (a + camera_pos.x<0 || a + camera_pos.x>MAP_SIZE - 1)continue;
            for (int b = 0; b < 20; b++) {
                if (b + camera_pos.y<0 || b + camera_pos.y>MAP_SIZE - 1)continue;
                for (int layer = 0; layer < 3; layer++) {
                    if (Map_data::get_inst()._maps[a+camera_pos.x][b+camera_pos.y]._id[layer]==-1)continue;
                    tileset_img.AlphaBlend(m_hBufferDC, a * 32, b * 32, 32, 32,
                        ((Map_data::get_inst()._maps[a+camera_pos.x][b+camera_pos.y]._id[layer]) % 33) * 16,
                        ((Map_data::get_inst()._maps[a+camera_pos.x][b+camera_pos.y]._id[layer]) / 33) * 16,
                        16, 16,255, AC_SRC_OVER);
                 
                }
                
            }
        }

        TCHAR coll[20];
        wsprintf(coll, L"내 위치: (%d,%d)",player->_x,player->_y);
        TextOut(m_hBufferDC, 400, 0, coll, lstrlen(coll));

        player->draw(m_hBufferDC);

        BitBlt(hdc, 0, 0, window_size, window_size, m_hBufferDC, 0, 0, SRCCOPY);
        EndPaint(hwnd, &ps);
        return 0;
    }
   
    case WM_KEYUP: {
 
        switch (wParam) {
        case 66:
            player->change_state(ATTACK, player->state_ptr->dir);
            break;
        case 67:
            player->change_state(DEATH, player->state_ptr->dir);
            break;
        case 68:
            player->change_state(HURT, player->state_ptr->dir);
            break;
        case VK_LEFT:
            player->change_state(IDLE, 2);
            break;
        case VK_RIGHT:
            player->change_state(IDLE, 3);
            break;
        case VK_UP:
            player->change_state(IDLE, 1);
            break;
        case VK_DOWN:
            player->change_state(IDLE, 0);
            break;
        }
        InvalidateRect(hwnd, NULL, FALSE);
    }
      return 0 ;
    case WM_KEYDOWN: {

        switch (wParam) {
        case VK_LEFT:
            player->change_state(WALK, 2);
            if (player->_x > 0 && Map_data::get_inst()._maps[player->_x - 1][player->_y]._collision != 1) {
                player->_x -= 1;
                camera_pos.x = ((player->state_ptr->render_pos_get(player->_x) == MAP_CULLING / 2)
                    && camera_pos.x > 0) ? camera_pos.x - 1 : camera_pos.x;
            } 
            break;
        case VK_RIGHT:
           player->change_state(WALK, 3);
            if (player->_x < MAP_SIZE - 1 && Map_data::get_inst()._maps[player->_x + 1][player->_y]._collision != 1) {
                player->_x += 1;
                camera_pos.x +=1;
            } 
            break;
        case VK_UP:
           player->change_state(WALK, 1);
            if (player->_y > 0 && Map_data::get_inst()._maps[player->_x][player->_y - 1]._collision != 1) {
                player->_y -= 1;
                camera_pos.y -=1;
            }
            break;
        case VK_DOWN:
            player->change_state(WALK,0);
            if (player->_y < MAP_SIZE - 1 && Map_data::get_inst()._maps[player->_x][player->_y + 1]._collision != 1) {
            
                player->_y += 1;
                camera_pos.y += 1;
            }
            break;
        }
        InvalidateRect(hwnd, NULL, FALSE);
        return 0;
    }
    case WM_LBUTTONDOWN: {

    }
      return 0;
   
    case WM_DESTROY:
      
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
