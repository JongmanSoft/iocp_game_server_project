#pragma once


#include "Scene.h"
#include "Map_data.h"
#include "object.h"


class Play_Scene :
    public Scene
{
public:
    CImage tileset_img;
    CImage profile_img;
    CImage hp_img;

    FMOD::System* g_system = nullptr;
    FMOD::Sound* g_bgm = nullptr;
    FMOD::Sound* g_attack = nullptr;
    FMOD::Sound* g_hurt = nullptr;
    FMOD::Sound* g_death = nullptr;
    FMOD::Channel* g_channel = nullptr;



    std::unique_ptr<object> player;
    std::unordered_map<long long, std::atomic<std::shared_ptr<object>>> objects;
public:
    HWND hEdit;

public:
    bool player_live = true;
private:
    // Chat log
    std::vector<std::wstring> chat_log;
    int chat_scroll_offset = 0; // Scroll offset in pixels
    const int chat_log_x = 41, chat_log_y = 395, chat_log_w = 180, chat_log_h = 130;
    const int line_height = 16; // Approximate height per line (font size 14 + spacing)

    // Chat input
    std::string chat_input;
    int cursor_pos = 0; // Cursor position in characters
    int input_scroll_offset = 0; // Horizontal scroll offset in pixels
    bool input_active = false; // Is the input area active?
    const int chat_input_x = 36, chat_input_y = 517, chat_input_w = 150, chat_input_h = 20;
    const int max_input_chars = 255;

    //std::wstring chat_input;
    bool composition_active = false;
    std::wstring composition_string;

    // 유틸리티 함수 선언 추가
    std::string WStringToUTF8(const std::wstring& wstr);
    std::wstring UTF8ToWString(const std::string& str);

    // Send button
    const int send_button_x = 186, send_button_y = 517, send_button_w = 20, send_button_h = 20;
    HBRUSH chat_brush = nullptr; // Brush for chat input and button background
    HFONT chat_font = nullptr; // Font for chat log and input
    HFONT button_font = nullptr; // Font for send button

public:
    Play_Scene(HWND hwnd, HBITMAP hBufferBitmap, HDC hBufferDC,HINSTANCE hinstance, void* fw);
    ~Play_Scene();
    void update() override;
    void network() override;
    void shutdown()override;
    LRESULT CALLBACK windowproc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) override;
    void Draw_chat();
    void add_chat_message(const std::wstring& message);
public:
    //네트워크
    void update_chat_log(TCHAR* chat);
    void change_state(int state);
};

