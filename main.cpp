#include <graphics.h>
#include <string>

int idx_cur_anim = 0;
const int ANIM_NUM = 5;

IMAGE img_player_left[ANIM_NUM];
IMAGE img_player_right[ANIM_NUM];

void loadanimation()
{
	for (size_t i = 0; i < ANIM_NUM; i++)
	{
		std::wstring path = L"img/player_left_"+std::to_wstring(i+1)+L".png";
		loadimage(&img_player_left[i], path.c_str());
	}
    for (size_t i = 0; i < ANIM_NUM; i++)
	{
		std::wstring path = L"img/player_right_"+std::to_wstring(i+1)+L".png";
		loadimage(&img_player_right[i], path.c_str());
	}
}

int main()
{
	initgraph(1280, 720);
	
	bool running = true;

	ExMessage msg;
	IMAGE img_background;

	loadimage(&img_background, _T("img/background.png"));
	loadanimation();

	BeginBatchDraw();

	while (running)
	{
		DWORD start_time = GetTickCount();
		while (peekmessage(&msg))
		{
			
		}

		static int counter = 0;
		if (++counter % 5 == 0) idx_cur_anim++;
		idx_cur_anim %= ANIM_NUM;

		cleardevice();

		putimage(0, 0, &img_background);
		putimage(500, 500, &img_player_right[idx_cur_anim]);

		FlushBatchDraw();

		DWORD end_time = GetTickCount();
		DWORD delta_time = end_time - start_time;
        if (delta_time < 1000/180) Sleep(1000/180 - delta_time);
	}

	EndBatchDraw();
}