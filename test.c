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
    typedef enum
    {
        Mainmenu,
        Playing,
        Pausemenu,
        Gameover
    } Gamestate;
    Gamestate state = Mainmenu;
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
        0.0f,     // attackcooldown
        100.f,    // health
        100.0f,   // maxhealth
        .5f,      // iframes
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
        false,
        0,
        0 // spiritcollision
    };
    Bull bulls[3] = {
        {1000.0f, 1800.0f, 100.0f, 2000.0f, 3500.0f, 90.0f, 20.0f, 1, 15000.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1, 1, Idle, true},
        {500.0f, 1800.0f, 100.0f, 2000.0f, 3500.0f, 90.0f, 20.0f, 1, 15000.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1, 1, Idle, true},
        {1500.0f, 1800.0f, 100.0f, 2000.0f, 3500.0f, 90.0f, 20.0f, 1, 15000.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1, 1, Idle, true},
    };
    Mimic mimics[3] = {
        {600.0f, 1800.0f, 0.0f, 10000.0f, 0.0f, 100.0f, 15.0f, 0.0f, 1.0f, 0.0f, 1, MIdle, true, {0}, false, 0.0f, 0.0f,1700.0f},//add amx speed at the end
        {900.0f, 1800.0f, 0.0f, 10000.0f, 0.0f, 100.0f, 15.0f, 0.0f, 1.0f, 0.0f, -1, MIdle, true, {0}, false, 0.0f, 0.0f,800.0f},
        {1200.0f, 1800.0f, 0.0f, 10000.0f, 0.0f, 100.0f, 15.0f, 0.0f, 1.0f, 0.0f, 1, MIdle, true, {0}, false, 0.0f, 0.0f,1200.0f},
    };
    int mimicCount = 3;
    int bullCount = 3; ////edited 0 for testing

    // float timer = 1; dont know what i used this for

    InitWindow(1440, 1080, "Title:The Name");
    SetExitKey(KEY_DELETE);
    HideCursor();
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
        if (state == Mainmenu)
        {
            if (IsKeyPressed(KEY_ENTER))
                state = Playing;
            BeginDrawing();
            ClearBackground(BLACK);
            DrawText("Press enter to start", screen_w / 2 - 500, screen_h / 2, 100, RED);
            EndDrawing();
        }
        if (state == Pausemenu)
        {
            if (IsKeyPressed(KEY_ENTER))
                state = Playing;
            if (IsKeyPressed(KEY_ESCAPE))
                state = Mainmenu;
            BeginDrawing();
            ClearBackground(BLACK);
            DrawText(TextFormat("This is a Pause menu"), screen_w / 2 - 600, screen_h / 2 - 200, 100, RED);
            DrawText(TextFormat("Press esc to go to the main menu\nPress enter to continue"), screen_w / 2 - 600, screen_h / 2, 50, RED);
            EndDrawing();
        }
        if (state == Playing)
        {
            if (IsKeyPressed(KEY_ESCAPE))
                state = Pausemenu;

            float dt = GetFrameTime();

            UpdateDash(&P, dt);

            UpdateMovementX(&P, dt);

            CollisionX(&P);

            UpdateJump(&P, dt);

            UpdateGravity(&P, dt); // gravity always has to be before collision y or jump wont work

            CollisionY(&P);

            AttackCheck = UpdateAttack(&P, dt, &AttackRect);

            for (int i = 0; i < bullCount; i++)
            {
                BullCollisionX(&bulls[i]);

                UpdateBullGravity(&bulls[i], dt);

                BullCollisionY(&bulls[i]);

                BullUpdateLogic(&bulls[i], &P, dt, AttackCheck, &AttackRect);

                CollisionX(&P);
            }
            int mimicattaks[mimicCount];
            for (int i = 0; i < mimicCount; i++)
            {
                MimicCollisionX(&mimics[i]);
                UpdateMimicGravity(&mimics[i], dt);
                MimicCollisionY(&mimics[i]);
                mimicattaks[i] = UpdateMimicLogic(&mimics[i], &P, dt, AttackCheck, &AttackRect);
            }

            spiritupdate(&en, &P, dt);

            CollisionX(&P);

            CollisionY(&P);
            if (P.health <= 0)
            {
                state = Gameover;
            }

            if (P.iframes > 0)
                P.iframes -= dt;

            // camera lerping starts
            camera.target.x += (P.x - camera.target.x) * 6.0f * dt;
            camera.target.y += (P.y - camera.target.y) * 6.0f * dt;
            // camera larping ends

            // drawing starts
            BeginDrawing();
            ClearBackground(BLACK);
            BeginMode2D(camera);
            if (P.iframes > 0 && (int)(P.iframes * 10) % 2 == 0)
                DrawRectangle(P.x, P.y, 100, 200, RED); // blinking during the invincibility frames
            else
                DrawRectangle(P.x, P.y, 100, 200, WHITE);
            for (int i = 0; i < bullCount; i++)
            {
                if (bulls[i].alive == true)
                    DrawRectangle(bulls[i].x, bulls[i].y, 200, 200, BLUE);
            }

            if (en.alive == true)
                DrawRectangle(en.x, en.y, 80, 80, RED);
            if (AttackCheck)
                DrawRectangleRec(AttackRect, RED);
            for (int i = 0; i < MAP_ROWS; i++)
            {
                for (int j = 0; j < MAP_COLS; j++)
                {
                    if (maps[currentLevel][i][j] == 1)
                        DrawRectangle((j * TILE_SIZE), (i * TILE_SIZE), TILE_SIZE, TILE_SIZE, GRAY);
                }
            }
            for (int i = 0; i < mimicCount; i++)
            {
                if (mimics[i].alive)
                {
                    DrawRectangle(mimics[i].x, mimics[i].y, 100, 200, GREEN);
                    if (mimicattaks[i])
                        DrawRectangleRec(mimics[i].attackrect, YELLOW);
                }
            }
            EndMode2D();
            DrawText(TextFormat("Dash Cooldown: %.1f", P.dashcooldown), 20, 40, 30, WHITE);
            DrawRectangle(20, 20, 200, 20, DARKGRAY);                       // health bar background grey
            DrawRectangle(20, 20, 200 * (P.health / P.maxHealth), 20, RED); // foreground — width = maxWidth * (health / maxHealth)
            EndDrawing();
        }

        if (state == Gameover)

        {
            if (IsKeyPressed(KEY_ENTER))
            {
                state = Mainmenu; // no type, just assignment
                P.x = 200.0f;
                P.y = 824.0f;
                P.health = 100.0f;
                P.velocityY = 0;
                P.iframes = 0;
                P.dashing = false;
                P.onground = true;
                P.doublejump = true;

                en.alive = true;
                en.x = 200.0f;
                en.y = 200.0f;
                en.spiritcollision = false;
                en.knockbackduration = 0;

                for (int i = 0; i < bullCount; i++)
                {
                    bulls[i].alive = true;
                    bulls[i].health = 90.0f;
                    bulls[i].state = Idle;
                    bulls[i].speed = 100.0f;
                }
                
            }
            BeginDrawing();
            ClearBackground(BLACK);
            DrawText("GAME OVER", screen_w / 2 - 150, screen_h / 2, 50, RED);
            DrawText("Press ENTER to restart", screen_w / 2 - 150, screen_h / 2 + 60, 30, WHITE);
            EndDrawing();
        }
    }

    CloseWindow();
    return 0;
}