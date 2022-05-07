/*******************************************************************************************
 *
 *   raylib [core] example - Basic window
 *
 *   Welcome to raylib!
 *
 *   To test examples, just press F6 and execute raylib_compile_execute script
 *   Note that compiled executable is placed in the same folder as .c file
 *
 *   You can find all basic examples on C:\raylib\raylib\examples folder or
 *   raylib official webpage: www.raylib.com
 *
 *   Enjoy using raylib. :)
 *
 *   This example has been created using raylib 1.0 (www.raylib.com)
 *   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
 *
 *   Copyright (c) 2013-2016 Ramon Santamaria (@raysan5)
 *
 ********************************************************************************************/

#include "raylib.h"
#include "perlin_noise.h"
#include "layer.h"
#include <math.h>

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

#define max(a, b) \
    ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })
#define min(a, b) \
    ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a < _b ? _a : _b; })

const int windowWidth = 800, windowHeight = 450;
const float moveSpeed = 1, zoomSpeed = .1f;
const float landmassTreshold = .5;
const Rectangle checkbox = {10, 10, 20, 20};
const Layer layers[] = {
    {.9f, {220, 255, 255, 255}},
    {.8f, {80, 50, 60, 255}},
    {.75f, BROWN},
    {.6f, GREEN},
    {.5f, YELLOW},
    {0, BLUE},
};
const char layerCount = 6;
const float acc = .9f;

Vector2 hexPos, mousePos, lastMousePos;
float rScale = 2;
float rX = 400;
float rY = 225;
float posXZoomed, posYZoomed;
float posX = 83333, posY = 83333;
float velX = 0, velY = 0;
float zoom = 80, noise;
float moveSpeedOverZoom;
bool debug = false;
Color c;

void Frame()
{
    // Update
    mousePos = GetMousePosition();
    moveSpeedOverZoom = moveSpeed / zoom;

    // Setting quality as per Framerate
    if (GetFrameTime() > .35f)
    {
       rScale *= 2;
       rX = windowWidth / rScale;
       rY = windowHeight / rScale;
    }

    // Movement Code

    // Keyboard Controls
    if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A))
        velX = -moveSpeedOverZoom;
    if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D))
        velX = moveSpeedOverZoom;
    if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W))
        velY = -moveSpeedOverZoom;
    if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S))
        velY = moveSpeedOverZoom;

    // Panning
    if (IsMouseButtonDown(0))
    {
        velX = (lastMousePos.x - mousePos.x) / zoom;
        velY = (lastMousePos.y - mousePos.y) / zoom;
    }

    // Applying velocity
    posX = max(min(posX + velX, 10000000 * zoom), 0);
    posY = max(min(posY + velY, 10000000 * zoom), 0);

    velX *= acc;
    velY *= acc;

    // Zooming using scroll wheel
    zoom *= pow(2, GetMouseWheelMove() * zoomSpeed);
    posXZoomed = posX * zoom;
    posYZoomed = posY * zoom;

    // Debug Code
    if (debug)
    {
        hexPos.x = mousePos.x + 400 - ((int)mousePos.x / (int)400) * 800;
        hexPos.y = mousePos.y;
    }

    // Debug checkbox Code
    if (CheckCollisionPointRec(mousePos, checkbox) && IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
        debug = !debug;

    // Draw
    BeginDrawing();
    for (int x = 0; x < rX / (1 + debug); x++)
        for (int y = 0; y < rY; y++)
        {
            noise = perlin2d(x + (int)(posXZoomed / rScale), y + (int)(posYZoomed / rScale), 1 / zoom, 1);

            for (char i = 0; i < layerCount; i++)
            {
                if (layers[i].height < noise)
                {
                    c = layers[i].color;
                    break;
                }
            }

            if (rScale == 1)
                DrawPixel(x, y, c);
            else
                DrawRectangle(x * rScale, y * rScale, rScale, rScale, c);

            if (debug)
            {
                c.r = c.g = c.b = noise * 255;
                if (rScale == 1)
                    DrawPixel(x + windowWidth / 2, y, c);
                else
                    DrawRectangle(x * rScale + windowWidth / 2, y * rScale, rScale, rScale, c);
            }
        }

    if (debug)
        DrawPoly(hexPos, 6, 5, 0, RED);

    DrawFPS(windowWidth - 80, 10);
    DrawText(TextFormat("Quality: %02.02f", GetFrameTime()), windowWidth / 2, 10, 20, BLACK);
    DrawText("Landmass Generation using Perlin Noise", 90, 50, 30, RED);
    DrawRectangle(checkbox.x - 2, checkbox.y - 2, checkbox.width + 4, checkbox.height + 4, WHITE);
    DrawRectangle(checkbox.x, checkbox.y, checkbox.width, checkbox.height, debug ? RED : GRAY);
    EndDrawing();

    lastMousePos = mousePos;
}

int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    InitWindow(windowWidth, windowHeight, "Landmass Generation");

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(Frame, 0, 1);
#else
    SetTargetFPS(60); // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
        Frame();
#endif

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow(); // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}