#include <graphics.h>
#include <string>
#include <windows.h>
#include <stdlib.h>
#include <vector>
#include <time.h>
#include "config.h"
#pragma comment(lib, "msimg32.lib") 
#pragma comment(lib, "Winmm.lib")

int idx_cur_anim = 0;
bool running = true;
bool is_game_started = false;
const int ANIM_NUM = 5;
const int WINDOW_WIDTH = GameConfig::Window::WIDTH;
const int WINDOW_HEIGHT = GameConfig::Window::HEIGHT;
const int BUTTON_WIDTH = GameConfig::Button::WIDTH;
const int BUTTON_HEIGHT = GameConfig::Button::HEIGHT;


class Atlas
{
public:
	Atlas(LPCTSTR path, int num)
	{
		TCHAR path_file[256];
		for (size_t i = 0; i < num; i++)
		{
			_stprintf_s(path_file, path, i);

			IMAGE* frame = new IMAGE();
			loadimage(frame, path_file);
			frame_list.push_back(frame);
		}
	}
	~Atlas()
	{
		for (size_t i = 0; i < frame_list.size(); i++)
		{
			delete frame_list[i];
		}
	}
public:
	std::vector<IMAGE*> frame_list;
};

Atlas* atlas_player_left;
Atlas* atlas_player_right;
Atlas* atlas_enemy_left;
Atlas* atlas_enemy_right;

ExMessage msg;
IMAGE img_menu;
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
	Animation(Atlas* atlas, int interval)
	{
		anim_atlas = atlas;
		interval_ms = interval;
	}
	~Animation() = default;

	void Play(int x, int y, int delta)
	{
		timer += delta;
		if (timer >= interval_ms)
		{
			idx_frame = (idx_frame + 1) % anim_atlas->frame_list.size();
			timer = 0;
		}

		putimage_alpha(x, y, anim_atlas->frame_list[idx_frame]);
	}
private:
	int timer = 0;
	int idx_frame = 0;
	int interval_ms = 0;
	Atlas* anim_atlas;
};

class Player
{
public:
	Player()
	{
		loadimage(&img_shadow, _T("img/shadow_player.png"));
		anim_left = new Animation(atlas_player_left, 45);
		anim_right = new Animation(atlas_player_right, 45);
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
				break;
			case VK_RIGHT:
				is_move_right = false;
				break;
			case VK_UP:
				is_move_up = false;
				break;
			case VK_DOWN:
				is_move_down = false;
				break;
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
	int PLAYER_SPEED = 3;
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
	POINT position = { 0,0 };
public:
	Bullet() = default;
	~Bullet() = default;

	void Draw() const {
		setlinecolor(GameConfig::Gameplay::BULLET_BORDER);
		setfillcolor(GameConfig::Gameplay::BULLET_COLOR);
		fillcircle(position.x, position.y,
			static_cast<int>(GameConfig::Gameplay::BULLET_RADIUS));
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
		anim_left = new Animation(atlas_enemy_left, 45);
		anim_right = new Animation(atlas_enemy_right, 45);

		enum class SpawnEdge
		{
			Up = 0,
			Down,
			Left,
			Right
		};

		time_t t;
		srand((unsigned)time(&t));
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

	void Hurt()
	{
		alive = false;
	}

	bool CheckAlive()
	{
		return alive;
	}

private:
	const int SPEED = 2;
	const int FRAME_WIDTH = 80;
	const int FRAME_HEIGHT = 80;
	const int SHADOW_WIDTH = 48;

	IMAGE img_shadow;
	Animation* anim_left;
	Animation* anim_right;
	POINT position = { 0, 0 };
	bool facing_left = false;
	bool alive = true;
};

class Button
{
public:
	Button(RECT rect, LPCTSTR path_img_idle, LPCTSTR path_img_hovered, LPCTSTR path_img_pushed)
	{
		region = rect;
		loadimage(&img_idle, path_img_idle);
		loadimage(&img_hovered, path_img_hovered);
		loadimage(&img_pushed, path_img_pushed);
	}

	~Button() = default;

	void ProcessEvent(const ExMessage& msg)
	{
		switch (msg.message)
		{
		case WM_MOUSEMOVE:
			if (status == Status::Idle && CheckCursorHit(msg.x, msg.y))
				status = Status::Hovered;
			else if (status == Status::Hovered && !CheckCursorHit(msg.x, msg.y))
				status = Status::Idle;
			break;
		case WM_LBUTTONDOWN:
			if (CheckCursorHit(msg.x, msg.y))
				status = Status::Pushed;
			break;
		case WM_LBUTTONUP:
			if (status == Status::Pushed)
				Onclick();
			break;
		default:
			break;
		}
	}

	void Draw()
	{
		switch (status)
		{
		case Status::Idle:
			putimage(region.left, region.top, &img_idle);
			break;
		case Status::Hovered:
			putimage(region.left, region.top, &img_hovered);
			break;
		case Status::Pushed:
			putimage(region.left, region.top, &img_pushed);
			break;
		}
	}

protected:
	virtual void Onclick() = 0;

private:
	enum class Status
	{
		Idle = 0,
		Hovered,
		Pushed
	};
private:
	RECT region;
	IMAGE img_idle;
	IMAGE img_hovered;
	IMAGE img_pushed;
	Status status = Status::Idle;
private:
	bool CheckCursorHit(int x, int y)
	{
		return x >= region.left && x <= region.right && y >= region.top && y <= region.bottom;
	}
};

class StartButton : public Button
{
public:
	StartButton(RECT rect, LPCTSTR path_img_idle, LPCTSTR path_img_hovered, LPCTSTR path_img_pushed)
		:Button(rect, path_img_idle, path_img_hovered, path_img_pushed)
	{
	}

	~StartButton() = default;

	void Onclick()
	{
		is_game_started = true;

		mciSendString(_T("play bgm repeat from 0"), NULL, 0, NULL);
	}
};

class ExitButton : public Button
{
public:
	ExitButton(RECT rect, LPCTSTR path_img_idle, LPCTSTR path_img_hovered, LPCTSTR path_img_pushed)
		:Button(rect, path_img_idle, path_img_hovered, path_img_pushed)
	{
	}

	~ExitButton() = default;

	void Onclick()
	{
		running = false;
	}
};

void TryGenerateEnemy(std::vector<Enemy*>& enemy_list)
{
	const int INTERVAL = GameConfig::Gameplay::SPAWN_INTERVAL;
	static int counter = 0;
	if ((++counter) % INTERVAL == 0) {
		enemy_list.push_back(new Enemy());
	}
}

void UpdateBullets(std::vector<Bullet>& bullet_list, const Player& player) {
	// 强制限制最大子弹数量
	if (bullet_list.size() > GameConfig::Gameplay::MAX_BULLETS) {
		bullet_list.resize(GameConfig::Gameplay::MAX_BULLETS);
	}

	const POINT& player_pos = player.GetPosition();
	const double radian_interval = 2 * 3.14159 / bullet_list.size();

	for (size_t i = 0; i < bullet_list.size(); i++) {
		const double radian = GetTickCount() * GameConfig::Gameplay::TANGENT_SPEED
			+ i * radian_interval;

		// 动态半径计算
		const double dynamic_radius = GameConfig::Gameplay::BASE_RADIUS
			+ 25 * sin(GetTickCount() * GameConfig::Gameplay::RADIAL_SPEED);

		bullet_list[i].position = {
			player_pos.x + player.PLAYER_WIDTH / 2 + static_cast<int>(dynamic_radius * sin(radian)),
			player_pos.y + player.PLAYER_HEIGHT / 2 + static_cast<int>(dynamic_radius * cos(radian))
		};
	}
}

void DrawPlayerScore(int score)
{
	static TCHAR text[64];
	_stprintf_s(text, _T("当前分数: %d"), score);

	setbkmode(TRANSPARENT);
	settextcolor(RGB(255, 255, 255));
	outtextxy(10, 10, text);
}