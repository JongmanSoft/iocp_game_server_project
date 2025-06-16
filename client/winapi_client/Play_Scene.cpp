#define _CRT_SECURE_NO_WARNINGS
#include "Play_Scene.h"
#include "Framework.h"
#include "network_data.h"

Play_Scene::Play_Scene(HWND hwnd, HBITMAP hBufferBitmap, HDC hBufferDC, HINSTANCE hinstance, void* fw)
{
    m_hwnd = hwnd;
    m_hBufferBitmap = hBufferBitmap;
    m_hBufferDC = hBufferDC;
    m_hinstance = hinstance;
    m_fw = fw;

    // 이미지 로드
    tileset_img.Load(L"image/tileset.png");
    profile_img.Load(L"image/profile.png");
    hp_img.Load(L"image/hp_bar.png");
    sys_mess_img.Load(L"image/system_msg.png");

    // 이미지 로드 확인
    if (tileset_img.IsNull()) OutputDebugString(L"Failed to load tileset_img!\n");
    if (profile_img.IsNull()) OutputDebugString(L"Failed to load profile_img!\n");
    if (hp_img.IsNull()) OutputDebugString(L"Failed to load hp_img!\n");

    Framework* FW = (Framework*)m_fw;
    player = std::make_unique<object>(PLAYER,
        FW->player_login_info.x, FW->player_login_info.y,
        FW->player_login_info.level, FW->player_login_info.hp,
        FW->player_login_info.name
    );

    // 사운드 초기화
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

    // 채팅 리소스 초기화
    chat_brush = CreateSolidBrush(RGB(90, 37, 23));
    chat_font = CreateFont(14, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
        DEFAULT_PITCH | FF_DONTCARE, L"둥근모꼴");
    button_font = CreateFont(11, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
        DEFAULT_PITCH | FF_DONTCARE, L"둥근모꼴");

}

Play_Scene::~Play_Scene()
{
    if (chat_brush) DeleteObject(chat_brush);
    if (chat_font) DeleteObject(chat_font);
    if (button_font) DeleteObject(button_font);
}

void Play_Scene::update()
{
    player->update();
    for (const auto& pair : objects) {
        std::shared_ptr<object> obj = pair.second.load();
        obj->update();
    }
    if (system_mess_time > 0) {
        system_mess_time += 1;
        if (system_mess_time > 20) system_mess_time = 0;
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
        g_channel = nullptr;
    }
    if (g_bgm != nullptr) {
        g_bgm->release();
        g_bgm = nullptr;
    }
    if (g_attack != nullptr) {
        g_attack->release();
        g_attack = nullptr;
    }
    if (g_hurt != nullptr) {
        g_hurt->release();
        g_hurt = nullptr;
    }
    if (g_death != nullptr) {
        g_death->release();
        g_death = nullptr;
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
        GetClientRect(hwnd, &rect);

        if (m_hBufferDC == nullptr) {
            OutputDebugString(L"Buffer DC is null!\n");
            EndPaint(hwnd, &ps);
            return 0;
        }

        // 배경
        FillRect(m_hBufferDC, &rect, (HBRUSH)GetStockObject(BLACK_BRUSH));
        SetBkMode(m_hBufferDC, TRANSPARENT);
        SetTextColor(m_hBufferDC, RGB(255, 255, 255));

        // 타일셋 그리기
        for (int a = 0; a < 20; a++) {
            if (a + (player->_x - 10) < 0 || a + (player->_x - 10) > MAP_SIZE - 1) continue;
            for (int b = 0; b < 20; b++) {
                if (b + (player->_y - 10) < 0 || b + (player->_y - 10) > MAP_SIZE - 1) continue;
                for (int layer = 0; layer < 3; layer++) {
                    if (Map_data::get_inst()._maps[a + (player->_x - 10)][b + (player->_y - 10)]._id[layer] == -1) continue;
                    tileset_img.AlphaBlend(m_hBufferDC, a * 32, b * 32, 32, 32,
                        ((Map_data::get_inst()._maps[a + (player->_x - 10)][b + (player->_y - 10)]._id[layer]) % 33) * 16,
                        ((Map_data::get_inst()._maps[a + (player->_x - 10)][b + (player->_y - 10)]._id[layer]) / 33) * 16,
                        16, 16, 255, AC_SRC_OVER);
                }
            }
        }

        // 오브젝트 그리기
        for (const auto& pair : objects) {
            std::shared_ptr<object> obj = pair.second.load();
            obj->draw(m_hBufferDC, player->_x, player->_y);
        }
        player->draw(m_hBufferDC);
        // 오브젝트 그리기
        for (const auto& pair : objects) {
            std::shared_ptr<object> obj = pair.second.load();
            if (obj->mess_ptr->current_frame > 0) obj->mess_ptr->render(m_hBufferDC,obj->_x, obj->_y, player->_x, player->_y);
        }

        // 폰트 선택
        HFONT old_font = (HFONT)SelectObject(m_hBufferDC, chat_font);

        // Profile UI
        {
            profile_img.AlphaBlend(m_hBufferDC, 0, 0, window_size, window_size,
                0, 0, profile_img.GetWidth(), profile_img.GetHeight(), 255, AC_SRC_OVER);
            int max_hp = MaxHP(player->_level);
            float hp_percent = ((float)player->_hp / (float)(max_hp)) * 72;
            hp_img.AlphaBlend(m_hBufferDC, 165, 86, hp_percent - 1, 9,
                0, 0, hp_img.GetWidth(), hp_img.GetHeight(), 255, AC_SRC_OVER);
            SetTextColor(m_hBufferDC, RGB(255, 255, 255));
            TCHAR hp_text[20];
            wsprintf(hp_text, L"%d / %d", player->_hp, max_hp);
            TextOut(m_hBufferDC, 170, 83, hp_text, lstrlen(hp_text));

            SetTextColor(m_hBufferDC, RGB(90, 37, 23));
            TextOutA(m_hBufferDC, 137, 26, player->_name, strlen(player->_name));

            wsprintf(hp_text, L"%d", player->_level);
            TextOut(m_hBufferDC, 172, 51, hp_text, lstrlen(hp_text));

            TCHAR coll[20];
            wsprintf(coll, L"(%d,%d)", player->_x, player->_y);
            TextOut(m_hBufferDC, 150, 100, coll, lstrlen(coll));
        }

        // Chat UI
        Draw_chat();

        //draw system msg
        if (system_mess_time > 0) {
            sys_mess_img.AlphaBlend(m_hBufferDC, 0, 0, window_size, window_size,
                0, 0, profile_img.GetWidth(), profile_img.GetHeight(), 255, AC_SRC_OVER);

            HFONT old_font = (HFONT)SelectObject(m_hBufferDC, chat_font);

            SetTextColor(m_hBufferDC, RGB(255, 255, 255));
            size_t char_count = 0; 
            size_t byte_pos = 0; 
            for (size_t i = 0; i < (out_sys_mess.length() + 19) / 20; i++) {
                std::string real_out_str;
                char_count = 0;
                size_t start_pos = byte_pos;
                while (byte_pos < out_sys_mess.length() && char_count < 20) {
                    unsigned char c = out_sys_mess[byte_pos];
                    if (c < 128) {
                        char_count++;
                        byte_pos++;
                    }
                    else {
                        char_count++;
                        byte_pos += 2;
                    }
                }
                real_out_str = out_sys_mess.substr(start_pos, byte_pos - start_pos);
                TextOutA(m_hBufferDC, 52, 401 + (i * 18), real_out_str.c_str(), real_out_str.length());
            }
            
            DeleteObject(old_font);
        }

        BitBlt(hdc, 0, 0, window_size, window_size, m_hBufferDC, 0, 0, SRCCOPY);
        SelectObject(m_hBufferDC, old_font);
        EndPaint(hwnd, &ps);
        return 0;
    }

    case WM_IME_COMPOSITION:
    {
        if (input_active) {
            HIMC hIMC = ImmGetContext(hwnd);
            if (hIMC) {
                if (lParam & GCS_RESULTSTR) {
                    // 완성된 문자열 가져오기
                    DWORD dwSize = ImmGetCompositionString(hIMC, GCS_RESULTSTR, NULL, 0);
                    if (dwSize > 0) {
                        std::vector<WCHAR> buffer(dwSize / sizeof(WCHAR) + 1);
                        ImmGetCompositionString(hIMC, GCS_RESULTSTR, &buffer[0], dwSize);
                        buffer[dwSize / sizeof(WCHAR)] = 0;

                        // 유니코드를 멀티바이트로 변환
                        int len = WideCharToMultiByte(CP_ACP, 0, &buffer[0], -1, nullptr, 0, nullptr, nullptr);
                        if (len > 0) {
                            std::vector<char> mbBuffer(len);
                            WideCharToMultiByte(CP_ACP, 0, &buffer[0], -1, &mbBuffer[0], len, nullptr, nullptr);

                            // 입력된 문자를 chat_input에 추가
                            std::string inputStr = &mbBuffer[0];
                            if (chat_input.length() + inputStr.length() < max_input_chars) {
                                chat_input.insert(cursor_pos, inputStr);
                                cursor_pos += inputStr.length();
                                InvalidateRect(hwnd, NULL, FALSE);
                            }
                        }
                    }
                }
                ImmReleaseContext(hwnd, hIMC);
            }
        }
        return 0;
    }

    case WM_IME_STARTCOMPOSITION:
    {
        if (input_active) {
            // IME 조합 시작
            return 0;
        }
        break;
    }

    case WM_IME_ENDCOMPOSITION:
    {
        if (input_active) {
            // IME 조합 종료
            return 0;
        }
        break;
    }

    case WM_CHAR: {
        if (input_active && wParam >= 32 && wParam != VK_RETURN && chat_input.length() < max_input_chars) {
            // ASCII 범위의 문자만 처리 (한글은 WM_IME_COMPOSITION에서 처리)
            if (wParam < 128) {  // ASCII 문자만
                chat_input.insert(cursor_pos, 1, (char)wParam);
                cursor_pos++;
                InvalidateRect(hwnd, NULL, FALSE);
            }
        }
        return 0;
    }
    case WM_KEYUP: {
        if (player_live) {
            switch (wParam) {
            case 65: // A
                NonBlockingClient::get_inst().sendAttackPacket(player->state_ptr->dir);
                break;
            case 68: // D 공격스킬
                NonBlockingClient::get_inst().sendUseSkill(ACTION_ATTACK_SKILL);
                break;
            case 83: // S
                NonBlockingClient::get_inst().sendUseSkill(ACTION_HEAL_SKILL);
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

        }
     
        InvalidateRect(hwnd, NULL, FALSE);
        return 0;
    }
    case WM_KEYDOWN: {
        if (input_active) {
            switch (wParam) {
            case VK_LEFT:
                if (cursor_pos > 0) cursor_pos--;
                InvalidateRect(hwnd, NULL, FALSE);
                break;
            case VK_RIGHT:
                if (cursor_pos < chat_input.length()) cursor_pos++;
                InvalidateRect(hwnd, NULL, FALSE);
                break;
            case VK_DELETE:
                if (cursor_pos < chat_input.length()) {
                    chat_input.erase(cursor_pos, 1);
                    InvalidateRect(hwnd, NULL, FALSE);
                }
                break;
            case VK_BACK:
                if (cursor_pos > 0) {
                    chat_input.erase(cursor_pos - 1, 1);
                    cursor_pos--;
                    InvalidateRect(hwnd, NULL, FALSE);
                }
                break;
            case VK_RETURN:
                // 채팅 전송 처리
                if (!chat_input.empty()) {
                    NonBlockingClient::get_inst().sendChatPacket(chat_input.c_str());
                    chat_input.clear();
                    cursor_pos = 0;
                    input_scroll_offset = 0;
                    InvalidateRect(hwnd, NULL, FALSE);
                }
                break;
            }
        }
        else {
            if (player_live) {
                switch (wParam) {
                case VK_LEFT:
                    player->change_state(WALK, 2);
                    NonBlockingClient::get_inst().sendStatePacket(WALK, MOVE_LEFT);
                    if (player->_x > 0 && Map_data::get_inst()._maps[player->_x - 1][player->_y]._collision != 1) {
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
            }
           
        }
        InvalidateRect(hwnd, NULL, FALSE);
        return 0;
    }
    case WM_LBUTTONDOWN: {
        int x = LOWORD(lParam);
        int y = HIWORD(lParam);

        // 채팅 입력창 클릭 처리
        if (x >= chat_input_x && x < chat_input_x + chat_input_w &&
            y >= chat_input_y && y < chat_input_y + chat_input_h) {
            input_active = true;
            HFONT old_font = (HFONT)SelectObject(m_hBufferDC, chat_font);
            int click_x = x - chat_input_x - 2 + input_scroll_offset;
            cursor_pos = 0;
            for (size_t i = 0; i < chat_input.length(); ++i) {
                std::string substr = chat_input.substr(0, i + 1);
                SIZE size;
                GetTextExtentPoint32A(m_hBufferDC, substr.c_str(), substr.length(), &size);
                if (size.cx > click_x) {
                    cursor_pos = i;
                    break;
                }
            }
            if (click_x > 0 && cursor_pos == 0 && chat_input.length() > 0) {
                cursor_pos = chat_input.length();
            }
            SelectObject(m_hBufferDC, old_font);
        }
        else {
            input_active = false;
        }

        // 전송 버튼 클릭 처리
        if (x >= send_button_x && x < send_button_x + send_button_w &&
            y >= send_button_y && y < send_button_y + send_button_h) {
            if (!chat_input.empty()) {
                NonBlockingClient::get_inst().sendChatPacket(chat_input.c_str());
                chat_input.clear();
                cursor_pos = 0;
                input_scroll_offset = 0;
                int total_height = chat_log.size() * line_height;
                if (total_height > chat_log_h) {
                    chat_scroll_offset = total_height - chat_log_h;
                }
                else {
                    chat_scroll_offset = 0;
                }
                InvalidateRect(hwnd, NULL, FALSE);
            }
        }
        return 0;
    }
    case WM_ERASEBKGND: {
        return 1;
    }
    case WM_DESTROY: {
        PostQuitMessage(0);
        return 0;
    }
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);

}

void Play_Scene::Draw_chat()
{
    HFONT old_font = (HFONT)SelectObject(m_hBufferDC, chat_font);
    SetBkMode(m_hBufferDC, TRANSPARENT);
    SetTextColor(m_hBufferDC, RGB(255, 255, 255));

    RECT input_rect = { chat_input_x, chat_input_y, chat_input_x + chat_input_w, chat_input_y + chat_input_h };
    SetTextColor(m_hBufferDC, RGB(255, 255, 255));
    std::string visible_text = chat_input;
    int text_width;
    SIZE size;
    GetTextExtentPoint32A(m_hBufferDC, chat_input.c_str(), chat_input.length(), &size);
    text_width = size.cx;

    if (text_width > chat_input_w - 4) {
        if (cursor_pos < chat_input.length()) {
            std::string to_cursor = chat_input.substr(0, cursor_pos + 1);
            GetTextExtentPoint32A(m_hBufferDC, to_cursor.c_str(), to_cursor.length(), &size);
            int cursor_x = size.cx;
            if (cursor_x - input_scroll_offset > chat_input_w - 4) {
                input_scroll_offset = cursor_x - (chat_input_w - 4);
            }
            else if (cursor_x - input_scroll_offset < 0) {
                input_scroll_offset = cursor_x;
            }
        }
        int start_char = 0;
        for (size_t i = 0; i < chat_input.length(); ++i) {
            std::string substr = chat_input.substr(0, i + 1);
            GetTextExtentPoint32A(m_hBufferDC, substr.c_str(), substr.length(), &size);
            if (size.cx >= input_scroll_offset) {
                start_char = i;
                break;
            }
        }
        visible_text = chat_input.substr(start_char);
        GetTextExtentPoint32A(m_hBufferDC, visible_text.c_str(), visible_text.length(), &size);
        while (size.cx > chat_log_w - 4 && !visible_text.empty()) {
            visible_text = visible_text.substr(0, visible_text.length() - 1);
            GetTextExtentPoint32A(m_hBufferDC, visible_text.c_str(), visible_text.length(), &size);
        }
    }
    else {
        input_scroll_offset = 0;
    }

    TextOutA(m_hBufferDC, chat_input_x + 2, chat_input_y + 2, visible_text.c_str(), visible_text.length());

    if (input_active) {
        std::string to_cursor = chat_input.substr(0, cursor_pos);
        GetTextExtentPoint32A(m_hBufferDC, to_cursor.c_str(), to_cursor.length(), &size);
        int cursor_x = chat_input_x + 2 + size.cx - input_scroll_offset;
        if (cursor_x >= chat_input_x + 2 && cursor_x < chat_input_x + chat_input_w - 2) {
            MoveToEx(m_hBufferDC, cursor_x, chat_input_y + 2, nullptr);
            LineTo(m_hBufferDC, cursor_x, chat_input_y + chat_input_h - 2);
        }
    }

    SelectObject(m_hBufferDC, button_font);
    SetTextColor(m_hBufferDC, RGB(255, 255, 255));
    RECT button_rect = { send_button_x, send_button_y, send_button_x + send_button_w, send_button_y + send_button_h };
    FillRect(m_hBufferDC, &button_rect, chat_brush);
    const char* send_text = "▼";
    TextOutA(m_hBufferDC, send_button_x + 8, send_button_y + 4, send_text, strlen(send_text));

    SelectObject(m_hBufferDC, old_font);
}

void Play_Scene::add_chat_message(const std::wstring& message)
{
    chat_log.push_back(message);
    int total_height = chat_log.size() * line_height;
    if (chat_scroll_offset >= total_height - chat_log_h - line_height || total_height <= chat_log_h) {
        chat_scroll_offset = total_height > chat_log_h ? total_height - chat_log_h : 0;
    }
    InvalidateRect(m_hwnd, NULL, FALSE);
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
        if (g_system && g_attack) {
            g_system->playSound(g_attack, nullptr, false, &g_channel);
        }
        player->change_state(ATTACK, player->state_ptr->dir);
        break;
    case DEATH:
        if (g_system && g_death) {
            g_system->playSound(g_death, nullptr, false, &g_channel);
        }
        player->change_state(DEATH, player->state_ptr->dir);
        break;
    case HURT:
        if (g_system && g_hurt) {
            g_system->playSound(g_hurt, nullptr, false, &g_channel);
        }
        player->_hp -= 2;
        player->change_state(HURT, player->state_ptr->dir);
        break;
    default:
        break;
    }
}