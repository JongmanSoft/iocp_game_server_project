#pragma once
#include "stdafx.h"
class chat_mess
{
public:
	CImage chat_img;
	char message[MAX_CHAT_LENGTH];
	unsigned short current_frame = 0;
public:
	chat_mess();
	void render(HDC m_hBufferDC);
	void render(HDC m_hBufferDC, int x, int y, int px, int py);
	void update();
	void mess_setting(const char mess[MAX_CHAT_LENGTH]);

	// std::string(UTF-8)을 std::wstring으로 변환
	std::wstring StringToWString(const std::string& str) {
		if (str.empty()) return std::wstring();
		int size = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
		if (size <= 0) return std::wstring();
		std::wstring wstr(size, L'\0');
		MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &wstr[0], size);
		return wstr;
	}
};

