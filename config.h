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
        //游戏设置
        constexpr int INIT_BULLETS = 3;
        constexpr int SPAWN_INTERVAL = 100;
    }
}