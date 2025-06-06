#include "Play_Scene.h"


Play_Scene::Play_Scene(HWND hwnd, HBITMAP hBufferBitmap, HDC hBufferDC, void* fw)
{
	m_hwnd = hwnd;
	m_hBufferBitmap = hBufferBitmap;
	m_hBufferDC = hBufferDC;
    m_fw = fw;
}

Play_Scene::~Play_Scene()
{

}

void Play_Scene::update()
{
}

void Play_Scene::network()
{
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
        //텍스트
        SetBkMode(m_hBufferDC, TRANSPARENT);
        SetTextColor(m_hBufferDC, RGB(236, 135, 138));
        TextOut(m_hBufferDC, 253, 466, L"무사히플레이로왓다", lstrlen(L"무사히플레이로왓다"));

        BitBlt(hdc, 0, 0, window_size, window_size, m_hBufferDC, 0, 0, SRCCOPY);
        EndPaint(hwnd, &ps);
        return 0;
    }
    case WM_CHAR: {

      
    }
     return 0;
    case WM_LBUTTONDOWN:
    {
       
    }
    return 0;
    case WM_KEYDOWN:

        InvalidateRect(hwnd, NULL, FALSE);
        return 0;
    case WM_DESTROY:
      
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
