#include <graphics.h>

int idx_cur_anim = 0;

int main()
{
	initgraph(1280, 720);

	bool running = true;

	ExMessage msg;
	IMAGE img_background;

	loadimage(&img_background, _T("img/background.png"));

	BeginBatchDraw();

	while (running)
	{
		while (peekmessage(&msg))
		{

		}

		static int counter = 0;

		cleardevice();
		putimage(0, 0, &img_background);
		FlushBatchDraw();
	}

	EndBatchDraw();
}