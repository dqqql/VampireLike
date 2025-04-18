#include "Game.h"
#include "config.h"
int main()
{
	initgraph(WINDOW_WIDTH, WINDOW_HEIGHT);

	atlas_player_left = new Atlas(GameConfig::Path::PLAYER_LEFT, 6);
    atlas_player_right = new Atlas(GameConfig::Path::PLAYER_RIGHT, 6);
    atlas_enemy_left = new Atlas(GameConfig::Path::ENEMY_LEFT, 6);
    atlas_enemy_right = new Atlas(GameConfig::Path::ENEMY_RIGHT, 6);

	mciSendString(_T("open mus/bgm.mp3 alias bgm"), NULL, 0, NULL);
    mciSendString(_T("open mus/hit.wav alias  hit"), NULL, 0, NULL);

	
	int score = 0;

	Player player;
	std::vector<Enemy*> enemy_list;
	std::vector<Bullet> bullet_list(GameConfig::Gameplay::BULLETS_NUM);
	RECT region_btn_start_game,region_btn_exit_game;

	region_btn_start_game.left = (WINDOW_WIDTH - BUTTON_WIDTH) / 2;
    region_btn_start_game.top = 430;
    region_btn_start_game.right = region_btn_start_game.left + BUTTON_WIDTH;
    region_btn_start_game.bottom = region_btn_start_game.top + BUTTON_HEIGHT;

	region_btn_exit_game.left = (WINDOW_WIDTH - BUTTON_WIDTH) / 2;
    region_btn_exit_game.top = 530;
    region_btn_exit_game.right = region_btn_exit_game.left + BUTTON_WIDTH;
    region_btn_exit_game.bottom = region_btn_exit_game.top + BUTTON_HEIGHT;

	StartButton btn_start_game = StartButton(region_btn_start_game, _T("img/ui_start_idle.png"), _T("img/ui_start_hovered.png"), _T("img/ui_start_pushed.png"));
	ExitButton btn_exit_game = ExitButton(region_btn_exit_game, _T("img/ui_quit_idle.png"), _T("img/ui_quit_hovered.png"), _T("img/ui_quit_pushed.png"));

	loadimage(&img_menu, _T("img/menu.png"));
	loadimage(&img_background, _T("img/background.png"));

	//初始化完成，接下来是游戏主循环

	BeginBatchDraw();

	while (running)
	{
		DWORD start_time = GetTickCount();
		while (peekmessage(&msg))
		{
			if (is_game_started)
				player.ProcessEvent(msg);
			else
			{
                btn_start_game.ProcessEvent(msg);
                btn_exit_game.ProcessEvent(msg);
			}
		}	
		if (is_game_started)
		{

			player.Move();
			player.frontiercheck();
			UpdateBullets(bullet_list, player);
			TryGenerateEnemy(enemy_list);
			for (Enemy* enemy : enemy_list)
				enemy->Move(player);

			for (Enemy* enemy : enemy_list)
			{
				if (enemy->CheckPlayerCollision(player))
				{
					MessageBoxW(GetHWnd(), _T("前面的关卡，以后再来探索吧"), _T("游戏结束"), MB_OK);
					running = false;
					break;
				}
			}
			for (Enemy* enemy : enemy_list)
			{
				for (const Bullet& bullet : bullet_list)
				{
					if (enemy->CheckBulletCollision(bullet))
					{
						mciSendString(_T("play hit from 0"), NULL, 0, NULL);
						enemy->Hurt();
						score++;
					}
				}
			}
			for (size_t i = 0; i < enemy_list.size();i++)
			{
				Enemy* enemy = enemy_list[i];
				if (!enemy->CheckAlive())
				{
					std::swap(enemy_list[i], enemy_list.back());
					enemy_list.pop_back();
					delete enemy;
				}
			}
		}
		cleardevice();
		if (is_game_started)
		{
			putimage(0, 0, &img_background);
			player.Draw(1000 / 180);
			for (Enemy* enemy : enemy_list)
				enemy->Draw(1000 / 180);
			for (const Bullet& bullet : bullet_list)
				bullet.Draw();
			DrawPlayerScore(score);
		}
		else
		{
            putimage(0, 0, &img_menu);
            btn_start_game.Draw();
            btn_exit_game.Draw();
		}
		FlushBatchDraw();

		DWORD end_time = GetTickCount();
		DWORD delta_time = end_time - start_time;
        if (delta_time < 1000/180) Sleep(1000/180 - delta_time);
	}

	delete atlas_player_left;
    delete atlas_player_right;
    delete atlas_enemy_left;
    delete atlas_enemy_right;

	EndBatchDraw();
}