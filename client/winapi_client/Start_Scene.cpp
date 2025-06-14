#include "stdafx.h"
#include "Start_Scene.h"
#include "network_data.h"

Start_Scene::Start_Scene(HWND hwnd, HBITMAP hBufferBitmap, HDC hBufferDC, void* fw) {
    m_hwnd = hwnd;
    m_hBufferBitmap = hBufferBitmap;
    m_hBufferDC = hBufferDC;
    m_fw = fw;
    title_img.Load(L"image/title.png");
    FMOD_RESULT result = FMOD::System_Create(&g_system);
    if (result == FMOD_OK) {
        result = g_system->init(32, FMOD_INIT_NORMAL, nullptr);
        if (result == FMOD_OK) {
            result = g_system->createSound("sound/BGM_title.mp3", FMOD_DEFAULT | FMOD_LOOP_NORMAL, nullptr, &g_bgm);
            if (result == FMOD_OK) {
                g_system->playSound(g_bgm, nullptr, false, &g_channel);
            }
        }
    }
}

Start_Scene::~Start_Scene() {
    
}



void Start_Scene::update() {

}

void Start_Scene::network() {
   
}

void Start_Scene::shutdown()
{
    if (g_channel != nullptr) {
        g_channel->stop();
    }
    if (g_bgm != nullptr) {
        g_bgm->release();
        g_bgm = nullptr;
    }
    if (g_system != nullptr) {
        g_system->close();
        g_system->release();
        g_system = nullptr;
    }
}

LRESULT Start_Scene::windowproc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
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
        // 타이틀이미지
        title_img.AlphaBlend(m_hBufferDC, 0, 0, window_size, window_size,
            0, 0, title_img.GetWidth(), title_img.GetHeight(),255, AC_SRC_OVER);
        //텍스트
        SetBkMode(m_hBufferDC, TRANSPARENT);
        SetTextColor(m_hBufferDC, RGB(90, 37, 23));

        TextOut(m_hBufferDC, 253, 467, IP_T.T_input, lstrlen(IP_T.T_input));
        TextOut(m_hBufferDC, 253, 511, ID_T.T_input, lstrlen(ID_T.T_input));
        BitBlt(hdc, 0, 0, window_size, window_size, m_hBufferDC, 0, 0, SRCCOPY);
        EndPaint(hwnd, &ps);
        return 0;
    }
    case WM_CHAR: {
            if (wParam == VK_RETURN) {
                std::string result;
                int len = WideCharToMultiByte(CP_UTF8, 0, IP_T.T_input, -1, nullptr, 0, nullptr, nullptr);
                if (len > 0) {
                    result.resize(len - 1);
                    WideCharToMultiByte(CP_UTF8, 0, IP_T.T_input, -1, &result[0], len, nullptr, nullptr);
                }
                NonBlockingClient::get_inst().init(result,GAME_PORT);
                NonBlockingClient::get_inst().connectToServer();
                scene_change(PLAY_SCENE);
            }
            if (!_caret.select) {
                if (wParam == VK_BACK) {
                    if (IP_T.count > 0) {
                        IP_T.T_input[IP_T.count - 1] = NULL;
                        IP_T.count--;
                    }
                }
                else if (IP_T.count < 20 && wParam != VK_RETURN) {
                    IP_T.T_input[IP_T.count++] = wParam;
                }
       
            }
            else {
                if (wParam == VK_BACK) {
                    if (ID_T.count > 0) {
                        ID_T.T_input[ID_T.count - 1] = NULL;
                        ID_T.count--;
                    }
                }
                else if (ID_T.count < 20 && wParam != VK_RETURN) {
                    ID_T.T_input[ID_T.count++] = wParam;
                }
               
            }
        
    }
            return 0;
    case WM_LBUTTONDOWN:
    {
        int mx = LOWORD(lParam);
        int my = HIWORD(lParam);

        POINT mypt = { mx,my };
        if (PtInRect(&IP_input_RT, mypt)) {
            _caret.select = 0;
        }
        if (PtInRect(&ID_input_RT, mypt)) {
            _caret.select = 1;

        }
    }
    return 0;
    case WM_KEYDOWN:
        
        InvalidateRect(hwnd, NULL, FALSE);
        return 0;
    case WM_DESTROY:
        // FMOD 자원 정리
        if (g_channel) g_channel->stop();
        if (g_bgm) g_bgm->release();
        if (g_system) {
            g_system->close();
            g_system->release();
        }
        PostQuitMessage(0);
        return 0;
        }
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
  }

