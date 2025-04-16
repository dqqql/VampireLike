#include <graphics.h>
#include <string>
#include <windows.h>
#include <stdlib.h>
#include <vector>
#include <time.h>
#pragma comment(lib, "msimg32.lib") 

int idx_cur_anim = 0;
const int ANIM_NUM = 5;
const int WINDOW_WIDTH = 1280, WINDOW_HEIGHT = 720;



ExMessage msg;
IMAGE img_background;
IMAGE img_player_left[5];
IMAGE img_player_right[5];


inline void putimage_alpha(int x, int y, IMAGE* img)
{
	int w = img->getwidth();
	int h = img->getheight();
	AlphaBlend(GetImageHDC(NULL), x, y, w, h,
		GetImageHDC(img), 0, 0, w, h, { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA });
}

class Animation
{
public:
	Animation(LPCTSTR path,int num ,int interval)
	{
		interval_ms = interval;

		TCHAR path_file[256];
		for (size_t i = 0;i < num; i++)
		{
			_stprintf_s(path_file, path, i);

			IMAGE* frame = new IMAGE();
			loadimage(frame, path_file);
            frame_list.push_back(frame);
		}
	}
    ~Animation()
	{
		for (size_t i = 0; i < frame_list.size(); i++)
            delete frame_list[i];
	}

	void Play(int x,int y,int delta)
	{
		timer += delta;
		if (timer > interval_ms)
		{
			idx_frame = (idx_frame+1)% frame_list.size();
			timer = 0;
		}

		putimage_alpha(x, y, frame_list[idx_frame]);
	}
private:
	int timer = 0;
	int idx_frame = 0;
	int interval_ms=0;
	std::vector<IMAGE*> frame_list;
};

class Player
{
public:
	Player()
    {
		loadimage(&img_shadow, _T("img/shadow_player.png"));
		anim_left = new Animation(_T("img/player_left_%d.png"), ANIM_NUM, 45);
        anim_right = new Animation(_T("img/player_right_%d.png"), ANIM_NUM, 45);
    }

    ~Player()
    {
        delete anim_left;
        delete anim_right;
    }

	void ProcessEvent(const ExMessage& msg)
    {
		if (msg.message == WM_KEYDOWN)
		{
			switch (msg.vkcode)
			{
			case VK_LEFT:
				is_move_left = true;
				break;
			case VK_RIGHT:
				is_move_right = true;
				break;
			case VK_UP:
				is_move_up = true;
				break;
			case VK_DOWN:
				is_move_down = true;
				break;
			}
		}
		else if (msg.message == WM_KEYUP)
		{
			switch (msg.vkcode)
			{
			case VK_LEFT:
				is_move_left = false;

			case VK_RIGHT:
				is_move_right = false;

			case VK_UP:
				is_move_up = false;

			case VK_DOWN:
				is_move_down = false;
			}
		}
    }

	void Move()
	{
		int dir_x = is_move_right - is_move_left;
		int dir_y = is_move_down - is_move_up;
		double len_dir = sqrt(dir_x * dir_x + dir_y * dir_y);
		if (len_dir != 0)
		{
			double normalized_x = dir_x / len_dir;
			double normalized_y = dir_y / len_dir;
			player_pos.x += (int)(PLAYER_SPEED * normalized_x);
			player_pos.y += (int)(PLAYER_SPEED * normalized_y);
		}


		/*if (is_move_left) player_pos.x -= PLAYER_SPEED;
		if (is_move_right) player_pos.x += PLAYER_SPEED;
		if (is_move_up) player_pos.y -= PLAYER_SPEED;
		if (is_move_down) player_pos.y += PLAYER_SPEED;*/
	}
	void frontiercheck()
	{
		
		if (player_pos.x < 0) player_pos.x = 0;
		if (player_pos.y < 0) player_pos.y = 0;
		if (player_pos.x + PLAYER_WIDTH > WINDOW_WIDTH) player_pos.x = WINDOW_WIDTH - PLAYER_WIDTH;
		if (player_pos.y + PLAYER_HEIGHT > WINDOW_HEIGHT) player_pos.y = WINDOW_HEIGHT - PLAYER_HEIGHT;
	}

	void Draw(int delta)
	{
		int dir_x = is_move_right - is_move_left;
		int pos_shadow_x = player_pos.x + (PLAYER_WIDTH / 2 - SHADOW_WIDTH / 2);
		int pos_shadow_y = player_pos.y + PLAYER_HEIGHT - 8;
		putimage_alpha(pos_shadow_x, pos_shadow_y, &img_shadow);

		static bool facing_left = false;
		if (dir_x < 0) facing_left = true;
		else if (dir_x > 0) facing_left = false;

		if (facing_left) anim_left->Play(player_pos.x, player_pos.y, delta);
		else anim_right->Play(player_pos.x, player_pos.y, delta);
	}

	const POINT& GetPosition() const
	{
		return player_pos;
	}
	
public:
	int PLAYER_SPEED = 4;
	const int PLAYER_WIDTH = 80;
	const int PLAYER_HEIGHT = 80;
	const int SHADOW_WIDTH = 32;

private:
	
	bool is_move_up = false;
	bool is_move_down = false;
	bool is_move_left = false;
	bool is_move_right = false;
	IMAGE img_shadow;
	POINT player_pos = { 500, 500 };
	Animation* anim_left;
    Animation* anim_right;
};

class Bullet
{
public:
	POINT position={0,0};
public:
	Bullet()= default;
	~Bullet()= default;

	void Draw() const
	{
		setlinecolor(RGB(255, 155, 50));
		setfillcolor(RGB(200, 75, 10));
		fillcircle(position.x, position.y, RADIUS);
	}

private:
	int RADIUS = 10;
};

class Enemy
{
public:
	Enemy()
	{
		loadimage(&img_shadow, _T("img/shadow_enemy.png"));
		anim_left = new Animation(_T("img/enemy_left_%d.png"), ANIM_NUM, 45);
        anim_right = new Animation(_T("img/enemy_right_%d.png"), ANIM_NUM, 45);

		enum class SpawnEdge
		{
			Up = 0,
			Down,
			Left,
            Right
		};

		time_t t;
		srand((unsigned) time(&t));
		SpawnEdge edge = (SpawnEdge)(rand() % 4);
		switch (edge)
		{
		case SpawnEdge::Up:
			position.y = -FRAME_HEIGHT;
            position.x = rand() % WINDOW_WIDTH;
			break;
        case SpawnEdge::Down:
            position.y = WINDOW_HEIGHT;
            position.x = rand() % WINDOW_WIDTH;
			break;
		case SpawnEdge::Left:
            position.x = -FRAME_WIDTH;
            position.y = rand() % WINDOW_HEIGHT;
			break;
		case SpawnEdge::Right:
            position.x = WINDOW_WIDTH;
            position.y = rand() % WINDOW_HEIGHT;
			break;
		}


	}
	~Enemy()
	{
		delete anim_left;
        delete anim_right;
	}

	bool CheckBulletCollision(const Bullet& bullet)
	{
		bool is_overlap_x = bullet.position.x >= position.x && bullet.position.x <= position.x + FRAME_WIDTH;
        bool is_overlap_y = bullet.position.y >= position.y && bullet.position.y <= position.y + FRAME_HEIGHT;
        return is_overlap_x && is_overlap_y;
	}

	bool CheckPlayerCollision(const Player& player)
	{
		POINT check_position = { position.x + FRAME_WIDTH / 2,position.y + FRAME_HEIGHT / 2 };
		const POINT& player_position = player.GetPosition();
		bool is_overlap_x = player_position.x <= check_position.x && check_position.x <= player_position.x + player.PLAYER_WIDTH;
		bool is_overlap_y = player_position.y <= check_position.y && check_position.y <= player_position.y + player.PLAYER_HEIGHT;
		return is_overlap_x && is_overlap_y;

	}

	void Move(const Player& player)
	{
		const POINT& player_pos = player.GetPosition();
		int dir_x = player_pos.x - position.x;
        int dir_y = player_pos.y - position.y;

		double len_dir = sqrt(dir_x * dir_x + dir_y * dir_y);
		if (len_dir != 0)
		{
			double normalized_x = dir_x / len_dir;
			double normalized_y = dir_y / len_dir;
			position.x += (int)(SPEED * normalized_x);
			position.y += (int)(SPEED * normalized_y);
		}

		if (dir_x < 0) facing_left = true;
        else if (dir_x > 0) facing_left = false;
	}

	void Draw(int delta)
	{
		int pos_shadow_x = position.x + (FRAME_WIDTH / 2 - SHADOW_WIDTH / 2);
        int pos_shadow_y = position.y + FRAME_HEIGHT - 35;
		putimage_alpha(pos_shadow_x, pos_shadow_y, &img_shadow);

		if (facing_left) anim_left->Play(position.x, position.y, delta);
        else anim_right->Play(position.x, position.y, delta);
	}

private:
	const int SPEED = 3;
	const int FRAME_WIDTH = 80;
	const int FRAME_HEIGHT = 80;
	const int SHADOW_WIDTH = 48;

	IMAGE img_shadow;
	Animation* anim_left;
    Animation* anim_right;
	POINT position = { 0, 0 };
	bool facing_left = false;
};

void TryGenerateEnemy(std::vector<Enemy*>& enemy_list)
{
	const int INTERVAL = 100;
	static int counter = 0;
	if ( (++counter) % INTERVAL == 0) {
		enemy_list.push_back(new Enemy());
	}
}

void UpdateBullets(std::vector<Bullet>& bullet_list, const Player& player)
{
	const double RADIAL_SPEED = 0.005;//径速
	const double TANGENT_SPEED = 0.005;//切速
	double radian_interval = 2*3.14159 / bullet_list.size();
	POINT player_pos = player.GetPosition();
	double radius = 100 + 25 * sin(GetTickCount() * RADIAL_SPEED);
    for (size_t i = 0; i < bullet_list.size(); i++)
	{
		double radian = GetTickCount() * TANGENT_SPEED + i * radian_interval;
		bullet_list[i].position.x = player_pos.x + player.PLAYER_WIDTH / 2 + (int)(radius * sin(radian));
        bullet_list[i].position.y = player_pos.y + player.PLAYER_HEIGHT / 2 + (int)(radius * cos(radian));
	}
}

int main()
{
	initgraph(WINDOW_WIDTH, WINDOW_HEIGHT);
	
	bool running = true;

	Player player;
	std::vector<Enemy*> enemy_list;
	std::vector<Bullet> bullet_list(3);

	loadimage(&img_background, _T("img/background.png"));

	BeginBatchDraw();

	while (running)
	{
		DWORD start_time = GetTickCount();
		while (peekmessage(&msg))
		{
			player.ProcessEvent(msg);
		}	
		player.Move();
		UpdateBullets(bullet_list, player);
		TryGenerateEnemy(enemy_list);
		for (Enemy* enemy : enemy_list)
			enemy -> Move(player);

		for (Enemy* enemy : enemy_list)
		{
			if (enemy->CheckPlayerCollision(player))
			{
				MessageBoxW(GetHWnd(), _T("You Lose"), _T("Game over"), MB_OK);
				running = false;
                break;
			}
		}

		cleardevice();
		putimage(0, 0, &img_background);
		player.Draw(1000/180);
		for (Enemy* enemy : enemy_list)
			enemy->Draw(1000/180);
		for (const Bullet& bullet : bullet_list)
			bullet.Draw();

		FlushBatchDraw();

		DWORD end_time = GetTickCount();
		DWORD delta_time = end_time - start_time;
        if (delta_time < 1000/180) Sleep(1000/180 - delta_time);
	}

	EndBatchDraw();
}