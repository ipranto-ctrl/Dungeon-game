#include <stdio.h>
#include <raylib.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include "player.h"
#include "tilemap.h"
#include "enemies.h"

typedef struct
{
    Rectangle rect;
    Color colour;
} platform;

Rectangle AttackRect;
int AttackCheck = 0;

int main(void)
{
    bool hitstate = false;

    Player P = {
        200.0f,   // x
        1000.0f,  // speed
        0.2f,     // dashtimer
        1,        // dashflag
        0.0f,     // dashcooldown
        824.0f,   // y
        10000.0f, // gravity
        0.0f,     // velocityY
        15,       // damage
        0,        // attackcooldown
        true,     // onground
        true,     // doublejump
        false,    // dashing
        true      // alive
    };
    Spirit en = {
        200.0f, // x
        200.0f, // y
        400.0f, // speed
        50.0f,  // damage
        0.0f,   // cooldown
        0.0f,   // knockbackduration
        true,   // alive
        false ,
        0,
        0  // spiritcollision
    };
    Bull bn = {
        1000.f,
        1800.0f,
        100.0f,
        2000.0f,
        3500.0f,
        500.0f,
        20.0f,
        1,
        15000.0f,
        0.0f,
        Idle,
        true};
    float timer = 1;

    InitWindow(1440, 1080, "Title:The Name");
    ToggleFullscreen();
    int screen_h = GetScreenHeight();
    int screen_w = GetScreenWidth();
    SetTargetFPS(60);

    // initialing the scrolling camera for the 1st frame
    Camera2D camera = {0};
    camera.target = (Vector2){P.x, P.y};                        // what it looks at
    camera.offset = (Vector2){screen_w / 2 - 50, screen_h / 2}; // where on screen
    camera.zoom = 0.8f;

    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();

        UpdateDash(&P, dt);

        UpdateMovementX(&P, dt);

        CollisionX(&P);

        UpdateJump(&P, dt);

        UpdateGravity(&P, dt);

        CollisionY(&P);

        AttackCheck = UpdateAttack(&P, dt, &AttackRect);

        BullCollisionX(&bn);

        UpdateBullGravity(&bn, dt);

        BullCollisionY(&bn);

        BullUpdateLogic(&bn, &P, dt, &timer, &bullhitimer, &hitstate, AttackCheck, &AttackRect);

        spiritupdate(&en, &P, dt);

        CollisionX(&P);

        CollisionY(&P);

        // camera lerping starts
        camera.target.x += (P.x - camera.target.x) * 6.0f * dt;
        camera.target.y += (P.y - camera.target.y) * 6.0f * dt;
        // camera larping ends

        // drawing starts
        BeginDrawing();
        ClearBackground(BLACK);
        BeginMode2D(camera);

        DrawRectangle(P.x, P.y, 100, 200, WHITE);
        DrawRectangle(bn.x, bn.y, 200, 200, BLUE);
        if (en.alive == true)
            DrawRectangle(en.x, en.y, 80, 80, RED);
        if (AttackCheck)
            DrawRectangleRec(AttackRect, RED);
        for (int i = 0; i < MAP_ROWS; i++)
        {
            for (int j = 0; j < MAP_COLS; j++)
            {
                if (map[i][j] == 1)
                    DrawRectangle((j * TILE_SIZE), (i * TILE_SIZE), TILE_SIZE, TILE_SIZE, GRAY);
            }
        }

        EndMode2D();
        DrawText(TextFormat("Dash Cooldown: %.1f", P.dashcooldown), 20, 20, 30, WHITE);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}