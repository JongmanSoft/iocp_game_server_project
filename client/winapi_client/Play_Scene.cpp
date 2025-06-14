#define _CRT_SECURE_NO_WARNINGS
#include "Play_Scene.h"
#include "Framework.h"
#include "network_data.h"

Play_Scene::Play_Scene(HWND hwnd, HBITMAP hBufferBitmap, HDC hBufferDC, void* fw)
{
    m_hwnd = hwnd;
    m_hBufferBitmap = hBufferBitmap;
    m_hBufferDC = hBufferDC;
    m_fw = fw;
    tileset_img.Load(L"image/tileset.png");
    profile_img.Load(L"image/profile.png");
    hp_img.Load(L"image/hp_bar.png");

    Framework* FW = (Framework*)m_fw;
    player = std::make_unique<object>(PLAYER, 
        FW->player_login_info.x, FW->player_login_info.y, 
        FW->player_login_info.level,FW->player_login_info.hp,
        FW->player_login_info.name
        );


    // Sound
    FMOD_RESULT result = FMOD::System_Create(&g_system);
    if (result == FMOD_OK) {
        result = g_system->init(32, FMOD_INIT_NORMAL, nullptr);
        if (result == FMOD_OK) {
            result = g_system->createSound("sound/BGM_play.mp3", FMOD_DEFAULT | FMOD_LOOP_NORMAL, nullptr, &g_bgm);
            if (result == FMOD_OK) {
                g_system->playSound(g_bgm, nullptr, false, &g_channel);
            }
            g_system->createSound("sound/attack.ogg", FMOD_DEFAULT, nullptr, &g_attack);
            g_system->createSound("sound/damage.ogg", FMOD_DEFAULT, nullptr, &g_hurt);
            g_system->createSound("sound/death.ogg", FMOD_DEFAULT, nullptr, &g_death);
        }
    }

    // Chat box
    //{
    //    // 채팅 로그용 읽기 전용 Edit 컨트롤 (투명 배경)
    //    CreateWindowEx(
    //        WS_EX_TRANSPARENT, // 확장 스타일
    //        L"EDIT", L"",
    //        WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY,
    //        41, 395, 180, 136, hwnd, (HMENU)1000,
    //        (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL
    //    );

    //    // 입력용 Edit 컨트롤
    //    hEdit = CreateWindowEx(
    //        WS_EX_TRANSPARENT, // 확장 스타일
    //        L"EDIT", L"",
    //        WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
    //        36, 547, 150, 20, hwnd, (HMENU)1001,
    //        (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL
    //    );

    //    // Send 버튼
    //    CreateWindow(
    //        L"BUTTON", L"전송",
    //        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
    //        186, 547, 40, 20, hwnd, (HMENU)1002,
    //        (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL
    //    );
    //}
}

Play_Scene::~Play_Scene()
{
}

void Play_Scene::update()
{
    player->update();
    //다른 오브젝트들
    for (const auto& pair : objects) {
        std::shared_ptr<object> obj = pair.second.load();
        obj->update();
    }
}

void Play_Scene::network()
{
    NonBlockingClient::get_inst().processNetwork();
}

void Play_Scene::shutdown()
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

LRESULT Play_Scene::windowproc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    static HBRUSH hWhiteBrush = CreateSolidBrush(RGB(255, 255, 255)); // 입력용 Edit 배경 브러시

    switch (uMsg) {
    case WM_CREATE:
    {
        return 0;
    }
    case WM_CTLCOLOREDIT: {
        HDC hdc = (HDC)wParam;
        HWND hEdit = (HWND)lParam;
        int ctrlID = GetDlgCtrlID(hEdit);

        if (ctrlID == 1000) { // 채팅 로그용 Edit 컨트롤
            SetBkMode(hdc, TRANSPARENT);
            SetTextColor(hdc, RGB(255, 255, 255)); // 흰색 글씨
            return (LRESULT)GetStockObject(NULL_BRUSH);
        }
        else if (ctrlID == 1001) { // 입력용 Edit 컨트롤
            SetBkMode(hdc, OPAQUE);
            SetBkColor(hdc, RGB(255, 255, 255)); // 흰색 배경
            SetTextColor(hdc, RGB(90, 37, 23)); // 다홍색 글씨
            return (LRESULT)hWhiteBrush;
        }
        return 0;
    }
    case WM_CTLCOLORBTN: {
        HDC hdc = (HDC)wParam;
        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, RGB(255, 255, 255));
        return (LRESULT)GetStockObject(NULL_BRUSH);
    }
    case WM_COMMAND: {
        if (LOWORD(wParam) == 1002 && HIWORD(wParam) == BN_CLICKED) {
            // Send 버튼 클릭
            HWND hInputEdit = GetDlgItem(hwnd, 1001);
            TCHAR buffer[256];
            GetWindowText(hInputEdit, buffer, 256); // 입력 텍스트 가져오기
            SetWindowText(hInputEdit, L""); // 입력창 비우기
            if (wcslen(buffer) > 0) { // 빈 입력 방지
                update_chat_log(buffer);
            }
        }
        return 0;
    }
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        RECT rect;
        GetClientRect(hwnd, &rect);

        // 배경
        FillRect(m_hBufferDC, &rect, (HBRUSH)GetStockObject(BLACK_BRUSH));
        SetBkMode(m_hBufferDC, TRANSPARENT);
        SetTextColor(m_hBufferDC, RGB(255, 255, 255));

        for (int a = 0; a < 20; a++) {
            if (a + (player->_x -10) < 0 || a + (player->_x - 10) > MAP_SIZE - 1) continue;
            for (int b = 0; b < 20; b++) {
                if (b + (player->_y - 10) < 0 || b + (player->_y - 10) > MAP_SIZE - 1) continue;
                for (int layer = 0; layer < 3; layer++) {
                    if (Map_data::get_inst()._maps[a + (player->_x - 10)][b + (player->_y - 10)]._id[layer] == -1) continue;
                    tileset_img.AlphaBlend(m_hBufferDC, a * 32, b * 32, 32, 32,
                        ((Map_data::get_inst()._maps[a + (player->_x -10)][b + (player->_y -10)]._id[layer]) % 33) * 16,
                        ((Map_data::get_inst()._maps[a + (player->_x -10)][b + (player->_y -10)]._id[layer]) / 33) * 16,
                        16, 16, 255, AC_SRC_OVER);
                }
            }
        }

        //다른 오브젝트들
        for (const auto& pair : objects) {
            std::shared_ptr<object> obj = pair.second.load();
            obj->draw(m_hBufferDC,player->_x,player->_y);
        }
        //나!
        player->draw(m_hBufferDC);

        // Profile UI
        {
            profile_img.AlphaBlend(m_hBufferDC, 0, 0, window_size, window_size,
                0, 0, profile_img.GetWidth(), profile_img.GetHeight(), 255, AC_SRC_OVER);

            float hp_percent = ((float)player->_hp / (float)(80 + (player->_level * 20))) * 72;
            hp_img.AlphaBlend(m_hBufferDC, 165, 86, hp_percent - 1, 9,
                0, 0, hp_img.GetWidth(), hp_img.GetHeight(), 255, AC_SRC_OVER);

            TCHAR hp_text[20];
            wsprintf(hp_text, L"%d / %d", player->_hp, 80 + (player->_level * 20));
            TextOut(m_hBufferDC, 170, 83, hp_text, lstrlen(hp_text));

            SetTextColor(m_hBufferDC, RGB(90, 37, 23));
            wchar_t wc[20];
            mbstowcs(wc, player->_name, 20);
            TextOut(m_hBufferDC, 137, 26, wc, lstrlen(wc));

            wsprintf(hp_text, L"%d", player->_level);
            TextOut(m_hBufferDC, 172, 51, hp_text, lstrlen(hp_text));

            TCHAR coll[20];
            wsprintf(coll, L"(%d,%d)", player->_x, player->_y);
            TextOut(m_hBufferDC, 150, 100, coll, lstrlen(coll));
        }

        BitBlt(hdc, 0, 0, window_size, window_size, m_hBufferDC, 0, 0, SRCCOPY);
        EndPaint(hwnd, &ps);
        return 0;
    }
    case WM_KEYUP: {
        switch (wParam) {
        case 65:
            player->mess_ptr->mess_setting("너 늬긔야 인간맞으면 대답해봐");
            break;
        case 66: // B
            change_state(ATTACK);
            break;
        case 67: // C
            change_state(HURT);
            break;
        case 68: // D
            change_state(DEATH);
            break;
        case VK_LEFT:
            NonBlockingClient::get_inst().sendStatePacket(IDLE, MOVE_LEFT);
            player->change_state(IDLE, 2);
            break;
        case VK_RIGHT:
            NonBlockingClient::get_inst().sendStatePacket(IDLE, MOVE_RIGHT);
            player->change_state(IDLE, 3);
            break;
        case VK_UP:
            NonBlockingClient::get_inst().sendStatePacket(IDLE, MOVE_UP);
            player->change_state(IDLE, 1);
            break;
        case VK_DOWN:
            NonBlockingClient::get_inst().sendStatePacket(IDLE, MOVE_DOWN);
            player->change_state(IDLE, 0);
            break;
        }
        InvalidateRect(hwnd, NULL, FALSE);
        return 0;
    }
    case WM_KEYDOWN: {
        switch (wParam) {
        case VK_LEFT:
            player->change_state(WALK, 2);
            NonBlockingClient::get_inst().sendStatePacket(WALK,MOVE_LEFT);
            if (player->_x > 0 && Map_data::get_inst()._maps[player->_x - 1][player->_y]._collision != 1) {
                //클라이언트에서 1차적으로 충돌검사
                NonBlockingClient::get_inst().sendMovePacket(MOVE_LEFT);
            }
            break;
        case VK_RIGHT:
            player->change_state(WALK, 3);
            NonBlockingClient::get_inst().sendStatePacket(WALK, MOVE_RIGHT);
            if (player->_x < MAP_SIZE - 1 && Map_data::get_inst()._maps[player->_x + 1][player->_y]._collision != 1) {
                NonBlockingClient::get_inst().sendMovePacket(MOVE_RIGHT);
            }
            break;
        case VK_UP:
            player->change_state(WALK, 1);
            NonBlockingClient::get_inst().sendStatePacket(WALK, MOVE_UP);
            if (player->_y > 0 && Map_data::get_inst()._maps[player->_x][player->_y - 1]._collision != 1) {
                NonBlockingClient::get_inst().sendMovePacket(MOVE_UP);
            }
            break;
        case VK_DOWN:
            player->change_state(WALK, 0);
            NonBlockingClient::get_inst().sendStatePacket(WALK, MOVE_DOWN);
            if (player->_y < MAP_SIZE - 1 && Map_data::get_inst()._maps[player->_x][player->_y + 1]._collision != 1) {
                NonBlockingClient::get_inst().sendMovePacket(MOVE_DOWN);
            }
            break;
        }
        InvalidateRect(hwnd, NULL, FALSE);
        return 0;
    }
    case WM_LBUTTONDOWN: {
        return 0;
    }
    case WM_ERASEBKGND: {
        return 1; // 깜빡임 방지
    }
    case WM_DESTROY: {
        DeleteObject(hWhiteBrush); // 브러시 해제
        PostQuitMessage(0);
        return 0;
    }
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void Play_Scene::update_chat_log(TCHAR* chat)
{
    HWND hLogEdit = GetDlgItem(m_hwnd, 1000);
    TCHAR log_buffer[1024];
    GetWindowText(hLogEdit, log_buffer, 1024); // 기존 로그 가져오기
    if (wcslen(log_buffer) > 0) {
        wcscat_s(log_buffer, L"\r\n"); // 줄바꿈 추가
    }
    wcscat_s(log_buffer, chat); // 새 메시지 추가
    SetWindowText(hLogEdit, log_buffer); // 로그 갱신

    // 스크롤을 맨 아래로 이동
    SendMessage(hLogEdit, EM_SETSEL, 0, -1);
    SendMessage(hLogEdit, EM_SETSEL, -1, -1);
    SendMessage(hLogEdit, EM_SCROLLCARET, 0, 0);

   
}

void Play_Scene::change_state(int state)
{
    switch (state)
    {
    case IDLE:
        player->change_state(IDLE, player->state_ptr->dir);
        break;
    case WALK:
        player->change_state(WALK, player->state_ptr->dir);
        break;
    case ATTACK:
        g_system->playSound(g_attack, nullptr, false, &g_channel);
        player->change_state(ATTACK, player->state_ptr->dir);
        break;
    case DEATH:
        g_system->playSound(g_death, nullptr, false, &g_channel);
        player->change_state(DEATH, player->state_ptr->dir);
        break;
    case HURT:
        g_system->playSound(g_hurt, nullptr, false, &g_channel);
        player->_hp -= 2;
        player->change_state(HURT, player->state_ptr->dir);
        break;
    default:
        break;
    }
}
