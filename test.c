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
        100.0f, // health
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
        false,   // alive
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
    Texture2D texIdle = LoadTexture("img/idle_right.png");
    Texture2D texSprint[4];
    texSprint[0] = LoadTexture("goth girl/right/right_sprint1.png");
    texSprint[1] = LoadTexture("goth girl/right/right_sprint2.png");
    texSprint[2] = LoadTexture("goth girl/right/right_sprint3.png");
    texSprint[3] = LoadTexture("goth girl/right/right_sprint4.png");
    Texture2D texJump[3];
    texJump[0] = LoadTexture("goth girl/Jump/Jump1.png");                   // launch
    texJump[1] = LoadTexture("goth girl/Jump/Jump2.png");                   // apex
    texJump[2] = LoadTexture("goth girl/Jump/Jump3.png");                   // falling
    Texture2D texDJumpBurst = LoadTexture("goth girl/Jump/DJumpBurst.png"); // one-shot flash
    Texture2D texDJumpParticles[3];
    texDJumpParticles[0] = LoadTexture("goth girl/Jump/DjumParticle1.png"); // burst begins
    texDJumpParticles[1] = LoadTexture("goth girl/Jump/DjumParticle2.png"); // peak
    texDJumpParticles[2] = LoadTexture("goth girl/Jump/DjumParticle3.png"); // fade out
    Texture2D texAttack[3];
    texAttack[0] = LoadTexture("goth girl/attack/attackframe1.png");
    texAttack[1] = LoadTexture("goth girl/attack/attackframe2.png");
    texAttack[2] = LoadTexture("goth girl/attack/attackframe3.png");
    Texture2D texAttackRect[2];
    texAttackRect[0] = LoadTexture("goth girl/attack/attackrect1.png");
    texAttackRect[1] = LoadTexture("goth girl/attack/attackrect2.png");

    // --- NEW: Animation Variables ---
    float sprintAnimTimer = 0.0f;
    int currentSprintFrame = 0;
    float doubleJumpFlashTimer = 0.0f;
    const float DOUBLE_JUMP_FLASH_DURATION = 0.15f; // how long the burst frame shows
    float doubleJumpParticleTimer = 0.0f;
    const float DOUBLE_JUMP_PARTICLE_DURATION = 0.3f; // total particle effect duration (all 3 frames)
    float doubleJumpParticleX = 0.0f;                 // position where the double jump was triggered (fixed in world space)
    float doubleJumpParticleY = 0.0f;
    float attackAnimTimer = 0.0f;
    int currentAttackFrame = 0;
    bool isAttacking = false;
    const float ATTACK_FRAME_DURATION = 0.08f;
    int attackDirection = 1; // facing direction locked in at the moment the attack starts; used for AttackRect sprite flip only

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
                    if (sprintAnimTimer >= 0.1f)
                    { // Swap frame every 0.1 seconds (10fps run cycle)
                        currentSprintFrame = (currentSprintFrame + 1) % 4;
                        sprintAnimTimer = 0.0f;
                    }
                }
                else
                {
                    sprintAnimTimer = 0.0f;
                    currentSprintFrame = 0;
                }

                UpdateSpikeKnockback(&P, dt);

                UpdateDash(&P, dt);

                UpdateMovementX(&P, dt);

                CollisionX(&P);

                bool wasDoubleJumpAvailable = P.doublejump;
                UpdateJump(&P, dt);
                if (wasDoubleJumpAvailable && !P.doublejump && !P.onground)
                {
                    doubleJumpFlashTimer = DOUBLE_JUMP_FLASH_DURATION;
                    doubleJumpParticleTimer = DOUBLE_JUMP_PARTICLE_DURATION;
                    doubleJumpParticleX = P.x;
                    doubleJumpParticleY = P.y;
                }

                UpdateGravity(&P, dt); // gravity always has to be before collision y or jump wont work

                CollisionY(&P);
                if (P.onground)
                {
                    doubleJumpFlashTimer = 0.0f;
                    doubleJumpParticleTimer = 0.0f;
                }

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
                if (en.alive == true)
                {
                    Rectangle src = {0, 0, spiritChase.width, spiritChase.height}; // reads the entire image, region of the image
                    Rectangle dest = {en.x - 20, en.y - 20, 100, 100};             // hitbox size, where to draw on screen(destination)
                    Vector2 spiritOrigin = {0, 0};                                 // pivot point of rotation
                    if (en.x > P.x)
                        src.width = -src.width; // for mirroring/flipping
                    DrawTexturePro(spiritChase, src, dest, spiritOrigin, 0.0f, WHITE);
                }

                if (AttackCheck)
                    DrawRectangleRec(AttackRect, RED);

                if (!P.onground)
                {
                    if (doubleJumpFlashTimer > 0.0f)
                    {
                        currentTex = texDJumpBurst;
                        doubleJumpFlashTimer -= dt;
                    }
                    else
                    {
                        // Pick jump frame from actual vertical velocity, not a timer,
                        // since jump duration varies with how high the player jumps.
                        // Both the first jump and the double jump (after its burst
                        // flash finishes) share this same travel animation.
                        if (P.velocityY < -800.0f)
                            currentTex = texJump[0]; // launch: still rising steeply
                        else if (P.velocityY > 800.0f)
                            currentTex = texJump[2]; // falling: descending fast
                        else
                            currentTex = texJump[1]; // apex: near the top of the arc
                    }
                }
                else
                {
                    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_D))
                    {
                        currentTex = texSprint[currentSprintFrame];
                    }
                    else
                    {
                        currentTex = texIdle;
                    }
                }
                if (AttackCheck)
                {
                    isAttacking = true;
                    currentAttackFrame = 0;
                    attackAnimTimer = 0.0f;
                    attackDirection = P.dashflag; // lock in facing direction for the whole swing, set only on the trigger frame
                }
                if (isAttacking)
                {
                    attackAnimTimer += dt;
                    if (attackAnimTimer >= ATTACK_FRAME_DURATION)
                    {
                        attackAnimTimer = 0.0f;
                        currentAttackFrame++;
                        if (currentAttackFrame >= 3)
                        {
                            currentAttackFrame = 2;
                            isAttacking = false;
                        }
                    }
                    currentTex = texAttack[currentAttackFrame];
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

                // Draw sprite larger than the 100x200 hitbox, anchored:
                // horizontally centered, vertically bottom-aligned (feet on the hitbox floor)
                float spriteDrawWidth = 250.0f;
                float spriteDrawHeight = 300.0f;
                float offsetX = (spriteDrawWidth - 100.0f) / 2.0f;
                float offsetY = spriteDrawHeight - 200.0f;
                Rectangle destRec = {P.x - offsetX, P.y - offsetY, spriteDrawWidth, spriteDrawHeight};

                // Determine blinking tint for iframes
                Color playerTint = WHITE;
                if (P.iframes > 0 && (int)(P.iframes * 10) % 2 == 0)
                {
                    playerTint = RED; // Blinks red on damage
                }

                // Draw the selected texture
                DrawTexturePro(currentTex, sourceRec, destRec, origin, 0.0f, playerTint);

                // Draw the double-jump particle burst at the fixed trigger position, if active
                if (doubleJumpParticleTimer > 0.0f)
                {
                    doubleJumpParticleTimer -= dt;
                    float elapsed = DOUBLE_JUMP_PARTICLE_DURATION - doubleJumpParticleTimer;
                    float progress = elapsed / DOUBLE_JUMP_PARTICLE_DURATION; // 0.0 to 1.0
                    int particleFrame = (int)(progress * 3.0f);
                    if (particleFrame > 2)
                        particleFrame = 2;
                    if (particleFrame < 0)
                        particleFrame = 0;

                    Texture2D particleTex = texDJumpParticles[particleFrame];
                    float particleDrawSize = 280.0f;
                    Rectangle particleSource = {0.0f, 0.0f, (float)particleTex.width, (float)particleTex.height};
                    // Centered on where the double jump was triggered, not the player's current position
                    Rectangle particleDest = {
                        doubleJumpParticleX + 50.0f - particleDrawSize / 2.0f,
                        doubleJumpParticleY,
                        particleDrawSize,
                        particleDrawSize};
                    DrawTexturePro(particleTex, particleSource, particleDest, (Vector2){0.0f, 0.0f}, 0.0f, WHITE);
                }

                for (int i = 0; i < bullCount; i++)
                {
                    if (bulls[i].alive == true)
                        DrawRectangle(bulls[i].x, bulls[i].y, 200, 200, BLUE);
                }

                if (en.alive == true)
                    DrawRectangle(en.x, en.y, 80, 80, RED);
                if (AttackCheck)
                {
                    // DrawRectangleRec(AttackRect, RED); actual attack
                    float rectSrcWidth = (float)texAttackRect[0].width;
                    if (attackDirection == -1)
                        rectSrcWidth = -rectSrcWidth;
                    Rectangle src = {0, 0, rectSrcWidth, texAttackRect[0].height};
                    Rectangle dest = {AttackRect.x, AttackRect.y, AttackRect.width, AttackRect.height};
                    DrawTexturePro(texAttackRect[0], src, dest, (Vector2){0, 0}, 0.0f, WHITE);
                }
                if (isAttacking && AttackCheck == 0)
                {
                    int rectFrame = (currentAttackFrame < 2) ? 0 : 1;
                    float rectSrcWidth = (float)texAttackRect[rectFrame].width;
                    if (attackDirection == -1)
                        rectSrcWidth = -rectSrcWidth; // flip horizontally, locked to the direction the attack started in
                    Rectangle src = {0, 0, rectSrcWidth, texAttackRect[rectFrame].height};
                    Rectangle dest = {AttackRect.x, AttackRect.y, AttackRect.width, AttackRect.height};
                    DrawTexturePro(texAttackRect[rectFrame], src, dest, (Vector2){0, 0}, 0.0f, WHITE);
                }
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
    }
    UnloadTexture(texIdle);
    for (int i = 0; i < 4; i++)
        UnloadTexture(texSprint[i]);
    for (int i = 0; i < 3; i++)
        UnloadTexture(texJump[i]);
    UnloadTexture(texDJumpBurst);
    for (int i = 0; i < 3; i++)
        UnloadTexture(texDJumpParticles[i]);
    UnloadTexture(spiritChase);
    for (int i = 0; i < 3; i++)
        UnloadTexture(texAttack[i]);
    for (int i = 0; i < 2; i++)
        UnloadTexture(texAttackRect[i]);
    CloseWindow();
    return 0;
}