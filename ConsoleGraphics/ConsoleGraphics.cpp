#include <Windows.h>
#include <string>
#include <chrono>
#include <iostream>

const int m_ScreenWidth = 120;
const int m_ScreenHeight = 40;

const int m_MapWidth = 16;
const int m_MapHeight = 16;

float player_x = 1.0f;
float player_y = 1.0f;
float player_a = 0.0f;

float FOV = 3.14f / 4.0f;
float Depth = 16.0f;
float Speed = 4.0f;


int main()
{
    wchar_t* screen = new wchar_t[m_ScreenHeight * m_ScreenWidth];
    HANDLE console = CreateConsoleScreenBuffer(GENERIC_READ|GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
    SetConsoleActiveScreenBuffer(console);
    DWORD dwBytesWritten = 0;

    /// Создаем карту игры
    std::wstring map;
    map += L"################";
    map += L"#..............#";
    map += L"#..............#";
    map += L"##########.....#";
    map += L"#..............#";
    map += L"#..............#";
    map += L"#..............#";
    map += L"#..............#";
    map += L"#..............#";
    map += L"#..............#";
    map += L"#..............#";
    map += L"#..............#";
    map += L"#..##.......##.#";
    map += L"#..............#";
    map += L"#..............#";
    map += L"################";

    auto timer1 = std::chrono::system_clock::now();
    auto timer2 = std::chrono::system_clock::now();

    while (1) 
    {
        timer2 = std::chrono::system_clock::now();

        std::chrono::duration<float> elapsedTime = timer2 - timer1;
        timer1 = timer2;
        float fElapsedTime = elapsedTime.count();

        if (GetAsyncKeyState((unsigned short)'A') & 0x8000)
            player_a -= (Speed)*fElapsedTime;
        if (GetAsyncKeyState((unsigned short)'D') & 0x8000)
            player_a += (Speed)*fElapsedTime;

        if (GetAsyncKeyState((unsigned short)'W') & 0x8000)
        {
            player_x += sinf(player_a) * Speed * fElapsedTime;
            player_y += cosf(player_a) * Speed * fElapsedTime;
            if (map.c_str()[(int)player_x + (int)player_y * m_MapWidth] == '#') {
                player_x -= sinf(player_a) * Speed * fElapsedTime;
                player_y -= cosf(player_a) * Speed * fElapsedTime;
            }
        }      

        if (GetAsyncKeyState((unsigned short)'S') & 0x8000)
        {
            player_x -= sinf(player_a) * Speed * fElapsedTime;
            player_y -= cosf(player_a) * Speed * fElapsedTime;
            if (map.c_str()[(int)player_x + (int)player_y * m_MapWidth] == '#') {
                player_x += sinf(player_a) * Speed * fElapsedTime;
                player_y += cosf(player_a) * Speed * fElapsedTime;
            }
        }

        for (int x = 0; x < m_ScreenWidth; ++x) {
            float ray_angle = player_a - FOV / 2 + ((float)x / (float)m_ScreenWidth) * FOV;

            bool bhit = false;

            float sight_x = std::sinf(ray_angle);
            float sight_y = std::cosf(ray_angle);

            float dist = 0.0f;
            float step = 0.1f;

            while (!bhit)
            {
                dist += step;

                int pos_x = (int)(player_x + sight_x * dist);
                int pos_y = (int)(player_y + sight_y * dist);

                if (pos_x < 0 || pos_x >= m_MapWidth || pos_y < 0 || pos_y >= m_MapHeight) {
                    bhit = true;
                    dist = Depth;
                }
                else
                {
                    if (map.c_str()[pos_y * m_MapWidth + pos_x] == '#') {
                        bhit = true;
                    }
                }
            }

            int Ceiling = (float)(m_ScreenHeight / 2.0f) - m_ScreenHeight / ((float)dist);
            int Floor = m_ScreenHeight - Ceiling;

            short Shade = ' ';
            if (dist <= Depth / 4.0f) Shade = 0x2588;
            else if (dist < Depth / 3.0f) Shade = 0x2593;
            else if (dist < Depth / 2.0f) Shade = 0x2592;
            else if (dist < Depth) Shade = 0x2591;
            else Shade = ' ';

            for (int y = 0; y < m_ScreenHeight; ++y) {
                if (y <= Ceiling) screen[y * m_ScreenWidth + x] = ' ';
                else if (y > Ceiling && y <= Floor) screen[y * m_ScreenWidth + x] = Shade;
                else
                {
                    float b = 1.0f - (((float)y - m_ScreenHeight / 2.0f) / ((float)m_ScreenHeight / 2.0f));
                    if (b < 0.25)		Shade = '#';
                    else if (b < 0.5)	Shade = 'x';
                    else if (b < 0.75)	Shade = '.';
                    else if (b < 0.9)	Shade = '-';
                    else				Shade = ' ';
                    screen[y * m_ScreenWidth + x] = Shade;
                }
            }
        }

        //screen[m_ScreenWidth * m_ScreenHeight - 1] = '\0';
        WriteConsoleOutputCharacter(console, screen, m_ScreenWidth * m_ScreenHeight, { 0,0 }, &dwBytesWritten);
    }

    return 0;
}
