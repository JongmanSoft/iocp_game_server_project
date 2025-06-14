#include "chat_mess.h"


chat_mess::chat_mess()
{
	chat_img.Load(L"image/chat_box.png");
}

void chat_mess::render(HDC m_hBufferDC)
{
	if (current_frame != 0) {
		
	
		int render_x = MAP_CULLING / 2;
		int render_y = MAP_CULLING / 2;
		chat_img.AlphaBlend(m_hBufferDC, 
			(render_x * 32) - 34 - (25),
			(render_y * 32) - 85 + ((100 -(20 * min(5, current_frame)))/2) -30,
			150,  30 * min(5, current_frame),
			0, 0, 100, 100, 255, AC_SRC_OVER);
		
		std::string out_str = message;
		if (current_frame > 5) {
			for (int i = 0; i < out_str.length() / 10; i++) {
				std::string cut_str = out_str.substr(i * 10, 10);
				size_t convertedChars = 0;
				std::wstring wstr(cut_str.length(), L'\0');
				wstr.assign(cut_str.begin(), cut_str.end());
				TextOut(m_hBufferDC, (render_x * 32) - 34 - (25) + 30
					, (render_y * 32) - 85 + ((100 - (20 * min(5, current_frame))) / 2) - 30 + 47
					+ (i * 12)
					, wstr.c_str(), lstrlen(wstr.c_str()));
			}
		}
		
		
	}
	
}

void chat_mess::render(HDC m_hBufferDC, int x, int y,int px, int py)
{
	if (current_frame != 0) {
		int render_x = x - (px - (MAP_CULLING / 2));
		int render_y = y - (py - (MAP_CULLING / 2));
		chat_img.AlphaBlend(m_hBufferDC,
			(render_x * 32) - 34 - (25),
			(render_y * 32) - 85 + ((100 - (20 * min(5, current_frame))) / 2) - 30,
			150, 30 * min(5, current_frame),
			0, 0, 100, 100, 255, AC_SRC_OVER);

		//메세지도..

	}
	
}

void chat_mess::update()
{
	(current_frame!= 0) ?current_frame++:current_frame;
	(current_frame > 40) ? current_frame = 0: current_frame;
}

void chat_mess::mess_setting(const char mess[MAX_CHAT_LENGTH])
{
	current_frame = 1;
	strcpy_s(message, mess);
}
