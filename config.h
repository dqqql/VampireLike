#pragma once

namespace GameConfig
{
    namespace Window
    {
        // 窗口大小
        constexpr int WIDTH = 1280;
        constexpr int HEIGHT = 720;
    }

    namespace Button
    {
        //按钮大小
        constexpr int WIDTH = 200;
        constexpr int HEIGHT = 50;
    }

    namespace Animation
    {
        //动画配置
        constexpr int NUM = 5;
        constexpr int FRAME_INTERVAL = 45;
    }

    namespace Path
    {
        // 玩家资源
        constexpr LPCTSTR PLAYER_LEFT = L"img/player_left_%d.png";
        constexpr LPCTSTR PLAYER_RIGHT = L"img/player_right_%d.png";
        constexpr LPCTSTR SHADOW_PLAYER = L"img/shadow_player.png";

        // 敌人资源
        constexpr LPCTSTR ENEMY_LEFT = L"img/enemy_left_%d.png";
        constexpr LPCTSTR ENEMY_RIGHT = L"img/enemy_right_%d.png";
        constexpr LPCTSTR SHADOW_ENEMY = L"img/shadow_enemy.png";

        // UI
        constexpr LPCTSTR BACKGROUND = L"img/background.png";
        constexpr LPCTSTR MENU = L"img/menu.png";

        // 音效
        constexpr LPCTSTR BGM = L"mus/bgm.mp3";
        constexpr LPCTSTR HIT_SOUND = L"mus/hit.wav";
    }

    namespace Gameplay
    {
        constexpr int SPAWN_INTERVAL = 100;

        constexpr int BULLETS_NUM = 4;    // 子弹数量
        constexpr int MAX_BULLETS = 8;     // 最大子弹数量
        constexpr float BULLET_RADIUS = 10.0f;
        constexpr COLORREF BULLET_COLOR = RGB(200, 75, 10);
        constexpr COLORREF BULLET_BORDER = RGB(255, 155, 50);

        // 子弹运动参数
        constexpr double RADIAL_SPEED = 0.004;  // 径向速度
        constexpr double TANGENT_SPEED = 0.003; // 切向速度
        constexpr double BASE_RADIUS = 100.0;   // 基础旋转半径
    }

    namespace Spawn {
        constexpr int SPAWN_INTERVAL_START = 100;   // 初始间隔（帧）
        constexpr int SPAWN_REDUCE_INTERVAL_SECONDS = 30; // 减少间隔（秒）
        constexpr int SPAWN_REDUCE_AMOUNT = 10;     // 每次减少量
        constexpr int SPAWN_MIN_INTERVAL = 40;      // 最小间隔
    }
}