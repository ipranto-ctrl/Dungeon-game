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
        200.0f,    // x
        1200.0f,   // speed
        0.2f,      // dashtimer
        1,         // dashflag
        0.0f,      // dashcooldown
        824.0f,    // y
        10000.0f,  // gravity
        0.0f,      // velocityY
        15,        // damage
        0.0f,      // attackcooldown
        100900.0f, // health
        100.0f,    // maxhealth
        .5f,       // iframes
        true,      // onground
        true,      // doublejump
        false,     // dashing
        true,      // alive
        0.0f,      // spikeknkbacktimer
        0          // spikeknkdirection
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
        {1000.0f, 1800.0f, 100.0f, 2500.0f, 3500.0f, 90.0f, 20.0f, 1, 15000.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1, 1, Idle, true, 1.0f},
        {500.0f, 1800.0f, 100.0f, 1500.0f, 3500.0f, 90.0f, 20.0f, 1, 15000.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1, 1, Idle, true, 0.5f},
        {1500.0f, 1800.0f, 100.0f, 4000.0f, 3500.0f, 90.0f, 20.0f, 1, 25000.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1, 1, Idle, true, 1.5f},
    };
    Mimic mimics[3] = {
        {600.0f, 1800.0f, 0.0f, 10000.0f, 0.0f, 100.0f, 15.0f, 0.0f, 1.0f, 0.0f, 1, MIdle, true, {0}, false, 0.0f, 0.0f, 1700.0f}, // add amx speed at the end
        {900.0f, 1800.0f, 0.0f, 10000.0f, 0.0f, 100.0f, 15.0f, 0.0f, 1.0f, 0.0f, -1, MIdle, true, {0}, false, 0.0f, 0.0f, 800.0f},
        {1200.0f, 1800.0f, 0.0f, 10000.0f, 0.0f, 100.0f, 15.0f, 0.0f, 1.0f, 0.0f, 1, MIdle, true, {0}, false, 0.0f, 0.0f, 1200.0f},
    };
    Archer archers[3] = {
        // x       y       velY  grav      spd  hp    dmg  atktimer jmptimer  dir  state  alive  onground  pKBtimer  KBdur  maxspd  arrowdmg
        {300.0f, 1800.0f, 0.0f, 10000.0f, 0.0f, 80.0f, 10.0f, 2.0f, 0.0f, 1, AIdle, true, false, 0.0f, 0.0f, 400.0f, 15.0f, 1.0f},
        {800.0f, 1800.0f, 0.0f, 10000.0f, 0.0f, 80.0f, 10.0f, 2.0f, 0.0f, -1, AIdle, true, false, 0.0f, 0.0f, 400.0f, 15.0f, 1.5f},
        {1400.0f, 1800.0f, 0.0f, 10000.0f, 0.0f, 80.0f, 10.0f, 2.0f, 0.0f, 1, AIdle, true, false, 0.0f, 0.0f, 400.0f, 15.0f, .5f},
    };
    int archerCount = 0;
    Arrow arrows[MAX_ARROWS] = {0}; // zero-init means all alive=false

    Totem totems[1] = {
        {1000.0f, 1800.0f, 60.0f, 10.0f, 5.0f, 1.5f, true, 0.0f},
        // x       y       health damage atktimer maxatktimer alive knockbackduration
    };
    int totemCount = 0;
    HomingBullet homingBullets[MAX_HOMING_BULLETS] = {0}; // zero-init means all alive=false

    int mimicCount = 0;
    int mimicattaks[mimicCount];
    int bullCount = 0; ////edited 0 for testing

    Dragon dragon = {
        1500.0f, // x
        500.0f,  // y
        50.0f,   // health (set below)
        30.0f,   // damage
        0.0f,    // chargetimer
        1.0f,    // maxchargetimer
        0.0f,    // attacktimer
        3.0f,    // maxattacktimer
        false,    // alive
        1,       // direction
        Didle,   // dstate
        {0},     // firerect
        0.0f,    // knockbackduration
        0.0f,    // playerknockbacktimer
        0.0f,    // playerecoil
        0,       // recoildirection
        1000.0f, // speed
        500.0f,  // attackspeed
        0.0f,    // wallDropSpeed
    };
    // dragon.health = 500.0f;

    // float timer = 1; dont know what i used this for

    InitWindow(1440, 1080, "Title:The Name");
    Texture2D spiritChase = LoadTexture("Sprite/Spirit_Chase100x100.png");
    SetExitKey(KEY_DELETE);
    HideCursor();
    ToggleFullscreen();
    int screen_h = GetScreenHeight();
    int screen_w = GetScreenWidth();
    SetTargetFPS(60);

    // --- NEW: Load Textures ---
    Texture2D texIdle = LoadTexture("img/char_idle.png");
    Texture2D texSprint1 = LoadTexture("img/char_sprint1.png");
    Texture2D texSprint2 = LoadTexture("img/char_sprint2.png");
    Texture2D texJump = LoadTexture("img/char_jump.png");
    Texture2D texDJump = LoadTexture("img/char_Djump.png");

    // --- NEW: Animation Variables ---
    float sprintAnimTimer = 0.0f;
    int currentSprintFrame = 1;

    // initialing the scrolling camera for the 1st frame
    Camera2D camera = {0};
    camera.target = (Vector2){P.x, P.y};                        // what it looks at
    camera.offset = (Vector2){screen_w / 2 - 50, screen_h / 2}; // where on screen
    camera.zoom = 0.7f;

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
            DrawText(TextFormat("Already Planning to"), screen_w / 2 - 600, screen_h / 2 - 200, 100, RED);
            DrawText(TextFormat("       Give up?"), screen_w / 2 - 600, screen_h / 2 - 100, 100, RED);
            DrawText(TextFormat("Press esc to go to the main menu\nPress enter to continue"), screen_w / 2 - 600, screen_h / 2, 50, RED);
            EndDrawing();
        }
        if (state == Playing)
        {
            if (IsKeyPressed(KEY_ESCAPE))
                state = Pausemenu;

            float dt = GetFrameTime();

            if (state == Playing)
            {
                float dt = GetFrameTime();

                // --- NEW: Update sprint animation timer ---
                if (IsKeyDown(KEY_A) || IsKeyDown(KEY_D))
                {
                    sprintAnimTimer += dt;
                    if (sprintAnimTimer >= 0.15f)
                    { // Swap frame every 0.15 seconds
                        currentSprintFrame = (currentSprintFrame == 1) ? 2 : 1;
                        sprintAnimTimer = 0.0f;
                    }
                }
                else
                {
                    sprintAnimTimer = 0.0f;
                    currentSprintFrame = 1;
                }

                UpdateSpikeKnockback(&P, dt);

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

                for (int i = 0; i < mimicCount; i++)
                {
                    MimicCollisionX(&mimics[i]);
                    UpdateMimicGravity(&mimics[i], dt);
                    MimicCollisionY(&mimics[i]);
                    mimicattaks[i] = UpdateMimicLogic(&mimics[i], &P, dt, AttackCheck, &AttackRect);
                }

                for (int i = 0; i < archerCount; i++)
                {
                    ArcherCollisionX(&archers[i]);
                    UpdateArcherGravity(&archers[i], dt);
                    ArcherCollisionY(&archers[i]);
                    UpdateArcherLogic(&archers[i], &P, dt, AttackCheck, &AttackRect, arrows, MAX_ARROWS);
                }

                DragonCollisionX(&dragon, dt);
                DragonCollisionY(&dragon);
                UpdateDragon(&dragon, &P, dt, AttackCheck, &AttackRect);

                UpdateArrows(arrows, MAX_ARROWS, &P, dt);

                for (int i = 0; i < totemCount; i++)
                {
                    TotemCollision(&totems[i], &P);
                    UpdateTotemLogic(&totems[i], &P, dt, AttackCheck, &AttackRect, homingBullets, MAX_HOMING_BULLETS);
                }
                UpdateHomingBullets(homingBullets, MAX_HOMING_BULLETS, &P, dt, AttackCheck, &AttackRect);

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
                // --- NEW: Determine which texture to draw ---
                Texture2D currentTex = texIdle;
                // if (P.iframes > 0 && (int)(P.iframes * 10) % 2 == 0)
                //     DrawRectangle(P.x, P.y, 100, 200, RED); // blinking during the invincibility frames
                // else
                //     DrawRectangle(P.x, P.y, 100, 200, WHITE);

                if (en.alive == true)
                {
                    Rectangle src = {0, 0, spiritChase.width, spiritChase.height}; // reads the entire image, region of the image
                    Rectangle dest = {en.x - 20, en.y - 20, 100, 100};             // hitbox size, where to draw on screen(destination)
                    Vector2 origin = {0, 0};                                      // pivot point of rotation
                    if (en.x > P.x)
                        src.width = -src.width; // for mirroring/flipping
                    DrawTexturePro(spiritChase, src, dest, origin, 0.0f, WHITE);
                }

                if (AttackCheck)
                    DrawRectangleRec(AttackRect, RED);

                if (!P.onground)
                {
                    if (!P.doublejump)
                    {
                        currentTex = texDJump;
                    }
                    else
                    {
                        currentTex = texJump;
                    }
                }
                else
                {
                    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_D))
                    {
                        currentTex = (currentSprintFrame == 1) ? texSprint1 : texSprint2;
                    }
                    else
                    {
                        currentTex = texIdle;
                    }
                }

                // --- NEW: Setup rectangles for drawing and flipping ---
                // A negative width in Raylib's sourceRec flips the texture horizontally
                float sourceWidth = (float)currentTex.width;

                Vector2 origin = {0.0f, 0.0f};
                if (P.dashflag == -1)
                {
                    sourceWidth = -sourceWidth; // Flip left
                }
                Rectangle sourceRec = {0.0f, 0.0f, sourceWidth, (float)currentTex.height};
                // Forcing it to fit the 100x200 dimensions you originally used for the hitbox
                Rectangle destRec = {P.x-50, P.y-80, 180.0f, 300.0f};

                // Determine blinking tint for iframes
                Color playerTint = WHITE;
                if (P.iframes > 0 && (int)(P.iframes * 10) % 2 == 0)
                {
                    playerTint = RED; // Blinks red on damage
                }

                // Draw the selected texture
                DrawTexturePro(currentTex, sourceRec, destRec, origin, 0.0f, playerTint);
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
                        if (maps[currentLevel][i][j] == 3)
                            DrawRectangle((j * TILE_SIZE), (i * TILE_SIZE), TILE_SIZE, TILE_SIZE, ORANGE); // spike
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
                for (int i = 0; i < archerCount; i++)
                {
                    if (archers[i].alive)
                        DrawRectangle(archers[i].x, archers[i].y, 100, 200, PURPLE);
                }
                for (int i = 0; i < totemCount; i++)
                {
                    if (totems[i].alive)
                        DrawRectangle(totems[i].x, totems[i].y, 100, 150, DARKPURPLE);
                }
                for (int i = 0; i < MAX_HOMING_BULLETS; i++)
                {
                    if (homingBullets[i].alive)
                        DrawCircle(homingBullets[i].x, homingBullets[i].y, 15, PINK);
                }
                if (dragon.alive)
                    DrawRectangle(dragon.x, dragon.y, 300, 200, DARKGREEN);
                if (dragon.dstate == Dattacking && dragon.alive == true)
                    DrawRectangleRec(dragon.firerect, ORANGE);
                for (int i = 0; i < MAX_ARROWS; i++)
                {
                    if (arrows[i].alive)
                        DrawCircle(arrows[i].x, arrows[i].y, 20, YELLOW);
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

                    for (int i = 0; i < mimicCount; i++)
                    {
                        mimics[i].alive = true;
                        mimics[i].health = 100.0f;
                        mimics[i].mstate = MIdle;
                        mimics[i].playerknockbacktimer = 0;
                        mimics[i].knockbackduration = 0;
                    }

                    for (int i = 0; i < bullCount; i++)
                    {
                        bulls[i].alive = true;
                        bulls[i].health = 90.0f;
                        bulls[i].state = Idle;
                        bulls[i].speed = 100.0f;
                    }
                    for (int i = 0; i < archerCount; i++)
                    {
                        archers[i].alive = true;
                        archers[i].health = 80.0f;
                        archers[i].Astate = AIdle;
                        archers[i].attacktimer = 2.0f;
                    }
                    for (int i = 0; i < MAX_ARROWS; i++)
                        arrows[i].alive = false;

                    dragon.alive = true;
                    dragon.health = 500.0f;
                    dragon.dstate = Didle;
                    dragon.x = 1500.0f;
                    dragon.y = 500.0f;
                    dragon.wallDropSpeed = 0;
                    dragon.playerknockbacktimer = 0;
                    dragon.playerecoil = 0;

                    for (int i = 0; i < totemCount; i++)
                    {
                        totems[i].alive = true;
                        totems[i].health = 60.0f;
                        totems[i].attacktimer = totems[i].maxattacktimer;
                        totems[i].knockbackduration = 0;
                    }
                    for (int i = 0; i < MAX_HOMING_BULLETS; i++)
                        homingBullets[i].alive = false;
                }
                BeginDrawing();
                ClearBackground(BLACK);
                DrawText("GAME OVER", screen_w / 2 - 150, screen_h / 2, 50, RED);
                DrawText("Press ENTER to restart", screen_w / 2 - 150, screen_h / 2 + 60, 30, WHITE);
                EndDrawing();
            }
        }
        // --- NEW: Unload textures before closing ---

        // CloseWindow();
        // return 0;
    }
    UnloadTexture(texIdle);
    UnloadTexture(texSprint1);
    UnloadTexture(texSprint2);
    UnloadTexture(texJump);
    UnloadTexture(texDJump);
    UnloadTexture(spiritChase);
    CloseWindow();
    return 0;
}