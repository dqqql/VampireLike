#pragma once

namespace GameConfig
{
    namespace Window
    {
        // ���ڴ�С
        constexpr int WIDTH = 1280;
        constexpr int HEIGHT = 720;
    }

    namespace Button
    {
        //��ť��С
        constexpr int WIDTH = 200;
        constexpr int HEIGHT = 50;
    }

    namespace Animation
    {
        //��������
        constexpr int NUM = 5;
        constexpr int FRAME_INTERVAL = 45;
    }

    namespace Path
    {
        // �����Դ
        constexpr LPCTSTR PLAYER_LEFT = L"img/player_left_%d.png";
        constexpr LPCTSTR PLAYER_RIGHT = L"img/player_right_%d.png";
        constexpr LPCTSTR SHADOW_PLAYER = L"img/shadow_player.png";

        // ������Դ
        constexpr LPCTSTR ENEMY_LEFT = L"img/enemy_left_%d.png";
        constexpr LPCTSTR ENEMY_RIGHT = L"img/enemy_right_%d.png";
        constexpr LPCTSTR SHADOW_ENEMY = L"img/shadow_enemy.png";

        // UI
        constexpr LPCTSTR BACKGROUND = L"img/background.png";
        constexpr LPCTSTR MENU = L"img/menu.png";

        // ��Ч
        constexpr LPCTSTR BGM = L"mus/bgm.mp3";
        constexpr LPCTSTR HIT_SOUND = L"mus/hit.wav";
    }

    namespace Gameplay
    {
        constexpr int SPAWN_INTERVAL = 100;

        constexpr int BULLETS_NUM = 4;    // �ӵ�����
        constexpr int MAX_BULLETS = 8;     // ����ӵ�����
        constexpr float BULLET_RADIUS = 10.0f;
        constexpr COLORREF BULLET_COLOR = RGB(200, 75, 10);
        constexpr COLORREF BULLET_BORDER = RGB(255, 155, 50);

        // �ӵ��˶�����
        constexpr double RADIAL_SPEED = 0.004;  // �����ٶ�
        constexpr double TANGENT_SPEED = 0.003; // �����ٶ�
        constexpr double BASE_RADIUS = 100.0;   // ������ת�뾶
    }

    namespace Spawn {
        constexpr int SPAWN_INTERVAL_START = 100;   // ��ʼ�����֡��
        constexpr int SPAWN_REDUCE_INTERVAL_SECONDS = 30; // ���ټ�����룩
        constexpr int SPAWN_REDUCE_AMOUNT = 10;     // ÿ�μ�����
        constexpr int SPAWN_MIN_INTERVAL = 40;      // ��С���
    }
}