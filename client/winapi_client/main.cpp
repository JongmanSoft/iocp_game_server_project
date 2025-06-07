#define _CRT_SECURE_NO_WARNINGS
#include "stdafx.h"
#include "Framework.h"
#include "Map_data.h"
#include "sprite_data.h"

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
DWORD WINAPI client_network(LPVOID param); 
DWORD WINAPI client_update(LPVOID param); 

HBITMAP hBufferBitmap;
HDC hBufferDC;
bool isRunning = true;
Framework* m_framework;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR lpCmdLine, int nCmdShow) {

    // 콘솔 창 열기
    AllocConsole();
    auto res = freopen("CONIN$", "r", stdin);  // 표준 입력을 콘솔로
    res = freopen("CONOUT$", "w", stdout); // 표준 출력을 콘솔로
    Map_data::get_inst().init();
    sprite_data::get_inst().init();

    WNDCLASSEX wcex = { 0 };
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WindowProc;
    wcex.hInstance = hInstance;
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.lpszClassName = L"JongmanRPG";
    if (!RegisterClassEx(&wcex)) return 1;

    HWND hwnd = CreateWindowEx(0, L"JongmanRPG", L"JongmanRPG", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, window_size, window_size, NULL, NULL, hInstance, NULL);
    if (hwnd == NULL) return 0;

    HDC hdc = GetDC(hwnd);
    hBufferDC = CreateCompatibleDC(hdc);
    RECT rect;
    GetClientRect(hwnd, &rect);
    hBufferBitmap = CreateCompatibleBitmap(hdc, rect.right, rect.bottom);
    SelectObject(hBufferDC, hBufferBitmap);
   
    ReleaseDC(hwnd, hdc);
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);
   
   HANDLE Network_Thread = CreateThread(NULL, 0, client_network, hwnd, 0, NULL);
   HANDLE Update_Thread = CreateThread(NULL, 0, client_update, hwnd, 0, NULL);

    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    isRunning = false;

    WaitForSingleObject(Network_Thread, INFINITE);
    CloseHandle(Network_Thread);
    WaitForSingleObject(Update_Thread, INFINITE); // 스레드가 종료될 때까지 대기
    CloseHandle(Update_Thread); // 스레드 핸들 닫기
   

    delete m_framework;
    DeleteDC(hBufferDC);
    DeleteObject(hBufferBitmap);

    return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

    if (uMsg == WM_CREATE) {
        HDC hdc = GetDC(hwnd);
        hBufferDC = CreateCompatibleDC(hdc);
        RECT rect;
        GetClientRect(hwnd, &rect);
        hBufferBitmap = CreateCompatibleBitmap(hdc, rect.right, rect.bottom);
        SelectObject(hBufferDC, hBufferBitmap);
        ReleaseDC(hwnd, hdc);
        m_framework = new Framework(hwnd, hBufferBitmap, hBufferDC);
      
        if (!m_framework) {
            return -1;
        }
      

        return 0;
    }
    if (uMsg == WM_DESTROY) { 

    }
    if (uMsg == WM_QUIT) {

    }

    if (m_framework) {
        return m_framework->windowproc(hwnd, uMsg, wParam, lParam);
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}



DWORD WINAPI client_update(LPVOID param) {
    HWND hwnd = (HWND)param;

    auto previousTime = std::chrono::high_resolution_clock::now();
    const double frameDuration = 1.0 / 10.0f; // 10 FPS

    while (isRunning) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        double elapsedTime = std::chrono::duration<double>(currentTime - previousTime).count();
   
        if (elapsedTime >= frameDuration) {
            previousTime = currentTime;
            m_framework->update();
            InvalidateRect(hwnd, NULL, FALSE);
        }
    }

    return 0;
}

DWORD WINAPI client_network(LPVOID param) {
    HWND hwnd = (HWND)param;

    auto previousTime = std::chrono::high_resolution_clock::now();
    const double frameDuration = 1.0 / 30.0f; // 30 FPS

    while (isRunning) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        double elapsedTime = std::chrono::duration<double>(currentTime - previousTime).count();

        if (elapsedTime >= frameDuration) {
            previousTime = currentTime;
            m_framework->network();
            InvalidateRect(hwnd, NULL, FALSE);
        }
    }

    return 0;
}