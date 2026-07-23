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

    //Spirit Texture Load
    Texture2D spiritChase = LoadTexture("Sprite/Spirit_Chase100x100.png");
    Texture2D spiritCharge= LoadTexture("Sprite/300x100_Charge_Up.png");
    Texture2D spiritStartBurst= LoadTexture("Sprite/100x100_Start2Burst.png");
    Texture2D spiritBurst= LoadTexture("Sprite/100x100_Burst.png");
    Texture2D spiritAfterBurst= LoadTexture("Sprite/300x100_After_Burst.png");

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
    Texture2D texAttackSide[3];
    texAttackSide[0] = LoadTexture("goth girl/attack/attackframe1.png");
    texAttackSide[1] = LoadTexture("goth girl/attack/attackframe2.png");
    texAttackSide[2] = LoadTexture("goth girl/attack/attackframe3.png");
    Texture2D texAttackUp[3];
    texAttackUp[0] = LoadTexture("goth girl/attack/attackframe1.png");
    texAttackUp[1] = LoadTexture("goth girl/attack/upattackframe2.png");
    texAttackUp[2] = LoadTexture("goth girl/attack/upattackframe3.png");
    Texture2D texAttackRect[2];
    texAttackRect[0] = LoadTexture("goth girl/attack/attackrect1.png");
    texAttackRect[1] = LoadTexture("goth girl/attack/attackrect2.png");
    Texture2D texDash[3];
    texDash[0] = LoadTexture("goth girl/right/dash1.png"); // windup / crouch
    texDash[1] = LoadTexture("goth girl/right/dash2.png"); // tucked burst, mid-dash
    texDash[2] = LoadTexture("goth girl/right/dash3.png"); // full speed streak

    // --- NEW: Animation Variables ---
    float sprintAnimTimer = 0.0f;
    int currentSprintFrame = 0;
    float doubleJumpFlashTimer = 0.0f;
    const float DOUBLE_JUMP_FLASH_DURATION = 0.15f; // how long the burst frame shows
    float doubleJumpParticleTimer = 0.0f;
    const float DOUBLE_JUMP_PARTICLE_DURATION = 0.3f; // total particle effect duration (all 3 frames)
    float doubleJumpParticleX = 0.0f;                 // position where the double jump was triggered (fixed in world space)
    float doubleJumpParticleY = 0.0f;
    int currentAttackFrame = 0;
    bool isAttacking = false;
    float attackCooldownAtTrigger = 0.0f; // cooldown value captured the instant this swing started; drives animation progress so the sprite can never outlast or finish before the real attack cooldown
    int attackDirection = 1;      // facing direction locked in at the moment the attack starts; used for AttackRect sprite flip only
    bool attackIsUpAttack = false; // whether the current swing is the up-attack, locked in at the moment the attack starts
    int currentDashFrame = 0;
    float dashDurationAtTrigger = 0.0f; // P.dashtimer value captured the instant the dash starts; drives animation progress the same way attackCooldownAtTrigger does
    float dashParticleX = 0.0f; // world-space launch point captured at dash start, so the burst stays put while the player rockets away from it
    float dashParticleY = 0.0f;

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

                bool wasDashing = P.dashing;
                UpdateDash(&P, dt);
                if (!wasDashing && P.dashing)
                {
                    dashDurationAtTrigger = P.dashtimer; // dash just started; whatever dashtimer holds right now is the full duration it'll count down from
                    dashParticleX = P.x;                 // fixed launch point, same idea as doubleJumpParticleX/Y
                    dashParticleY = P.y;
                }

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

                //Spirit Sprites
                if (en.alive == true)
                {
                    Texture2D currentSpiritTex= spiritChase;
                    int frames=1;
                    int currentFrame= 0;
                    float scaleSize= 100.0f; //Baseline Dimension Scaling
                    float offsetX= -25.0f; //(hitbox 50/2) - (scaleSize 100/2)
                    float offsetY= -25.0f;

                    if (en.spiritcollision == true && en.knockbackduration <= 0)
                    {
                        // Phase 2: Charging up (en.cooldown ticks down from 0.5 to 0)
                        if (en.cooldown > 0.1f)
                        {
                            currentSpiritTex = spiritCharge;
                            frames = 3;
                            float timeElapsed = 0.5f - en.cooldown; // Ranges from 0.0 to 0.4
                            currentFrame = (int)(timeElapsed / (0.4f / 3.0f));
                            if (currentFrame > 2) currentFrame = 2;
                        }
                        else
                        {
                            currentSpiritTex = spiritStartBurst;
                            frames = 1;
                            currentFrame = 0;
                            scaleSize = 300.0f; // Scale up for tension,, burst boro choto hoy
                            offsetX = -125.0f; // (50/2) - (300/2)
                            offsetY = -125.0f;
                        }
                    }
                    else if (en.knockbackduration > 0)
                    {
                        // Phase 3: Burst and Recoil (en.knockbackduration ticks from 0.3 down to 0)
                        if (en.knockbackduration > 0.2f)
                        {
                            currentSpiritTex = spiritBurst;
                            frames = 1;
                            currentFrame = 0;
                            scaleSize = 400.0f; // Explosion expands past hitbox edges
                            offsetX = -175.0f; // (50/2) - (400/2)
                            offsetY = -175.0f;
                        }
                        else
                        {
                            currentSpiritTex = spiritAfterBurst;
                            frames = 3;
                            float timeElapsed = 0.2f - en.knockbackduration; // Ranges from 0.0 to 0.2
                            currentFrame = (int)(timeElapsed / (0.2f / 3.0f));
                            if (currentFrame > 2) currentFrame = 2;
                            scaleSize = 400.0f; 
                            offsetX = -175.0f;
                            offsetY = -175.0f;
                        }
                    }

                    float frameWidth = (float)currentSpiritTex.width / frames;
                    Rectangle src = {currentFrame * frameWidth, 0, frameWidth, (float)currentSpiritTex.height};
                    
                    // Flip texture based on direction if it's currently chasing
                    if (!en.spiritcollision && en.x > P.x)
                        src.width = -src.width; 

                    Rectangle dest = {en.x + offsetX, en.y + offsetY, scaleSize, scaleSize};
                    Vector2 spiritOrigin = {0, 0};
                    DrawTexturePro(currentSpiritTex, src, dest, spiritOrigin, 0.0f, WHITE);
                }

                if (AttackCheck)
                    DrawRectangleRec(AttackRect, RED);

                if (P.dashing)
                {
                    // Same progress-from-real-timer trick as the attack animation below:
                    // derive the frame from how much of the actual dash duration has
                    // elapsed so the sprite can't finish before or outlast the dash itself.
                    float dashProgress = (dashDurationAtTrigger > 0.0f)
                        ? (1.0f - (P.dashtimer / dashDurationAtTrigger))
                        : 1.0f;
                    currentDashFrame = (int)(dashProgress * 3.0f);
                    if (currentDashFrame > 2)
                        currentDashFrame = 2;
                    if (currentDashFrame < 0)
                        currentDashFrame = 0;

                    currentTex = texDash[currentDashFrame];
                }
                else if (!P.onground)
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
                    attackCooldownAtTrigger = P.attackcooldown; // cooldown UpdateAttack just set for this swing (e.g. 0.25)
                    attackDirection = P.dashflag;                // lock in facing direction for the whole swing, set only on the trigger frame
                    attackIsUpAttack = IsKeyDown(KEY_W);         // same condition UpdateAttack used internally to build the up-attack AttackRect
                }
                if (isAttacking)
                {
                    // Derive animation progress directly from how much of the real
                    // attack cooldown has elapsed, so the sprite can never finish
                    // before or outlast the actual cooldown used by the hitbox logic.
                    float progress = (attackCooldownAtTrigger > 0.0f)
                        ? (1.0f - (P.attackcooldown / attackCooldownAtTrigger))
                        : 1.0f;
                    currentAttackFrame = (int)(progress * 3.0f);
                    if (currentAttackFrame > 2)
                        currentAttackFrame = 2;
                    if (currentAttackFrame < 0)
                        currentAttackFrame = 0;

                    if (P.attackcooldown <= 0.0f)
                        isAttacking = false;

                    currentTex = attackIsUpAttack ? texAttackUp[currentAttackFrame] : texAttackSide[currentAttackFrame];
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

                // Reuse the double-jump particle burst for the dash: same 3 frames,
                // driven off the same progress trick as the dash sprite itself, but
                // rotated 90 degrees counterclockwise (raylib rotation is clockwise-
                // positive, so -90 gives CCW) and drawn bigger. Anchored at the fixed
                // launch point captured when the dash started, same idea as the
                // double-jump particle staying put at doubleJumpParticleX/Y.
                if (P.dashing)
                {
                    float dashParticleProgress = (dashDurationAtTrigger > 0.0f)
                        ? (1.0f - (P.dashtimer / dashDurationAtTrigger))
                        : 1.0f;
                    int dashParticleFrame = (int)(dashParticleProgress * 3.0f);
                    if (dashParticleFrame > 2)
                        dashParticleFrame = 2;
                    if (dashParticleFrame < 0)
                        dashParticleFrame = 0;

                    Texture2D dashParticleTex = texDJumpParticles[dashParticleFrame];
                    float dashParticleDrawSize = 280.0f * 1.5f; // larger than the double-jump version
                    Rectangle dashParticleSource = {0.0f, 0.0f, (float)dashParticleTex.width, (float)dashParticleTex.height};
                    // Origin at the sprite's own center so the -90 rotation pivots in place
                    // instead of swinging around a corner.
                    Vector2 dashParticleOrigin = {dashParticleDrawSize / 2.0f, dashParticleDrawSize / 2.0f};
                    // dest.x/y is where that center-origin lands in world space, so this
                    // centers the burst on the player's hitbox at the launch point.
                    Rectangle dashParticleDest = {
                        dashParticleX + 50.0f,
                        dashParticleY + 100.0f,
                        dashParticleDrawSize,
                        dashParticleDrawSize};
                    DrawTexturePro(dashParticleTex, dashParticleSource, dashParticleDest, dashParticleOrigin, -90.0f, WHITE);
                }

                for (int i = 0; i < bullCount; i++)
                {
                    if (bulls[i].alive == true)
                        DrawRectangle(bulls[i].x, bulls[i].y, 200, 200, BLUE);
                }

                // if (en.alive == true)
                //     DrawRectangle(en.x, en.y, 80, 80, RED);

                if (AttackCheck)
                {
                    // DrawRectangleRec(AttackRect, RED); actual attack
                    // Up-attack needs the hitbox sprite rotated 90 degrees
                    // counterclockwise (raylib rotation is clockwise-positive,
                    // so -90 gives CCW). Width/height are swapped before
                    // rotating so the post-rotation footprint still matches
                    // AttackRect's actual shape, and the rotation pivots
                    // around AttackRect's center instead of its corner.
                    // For the left-facing flip: negating width flips
                    // horizontally BEFORE rotation, which becomes a vertical
                    // flip once the -90 rotation swaps the axes. So up-attacks
                    // flip height instead, which reads correctly as
                    // horizontal after rotation.
                    float rectSrcWidth = (float)texAttackRect[0].width;
                    float rectSrcHeight = (float)texAttackRect[0].height;
                    if (attackDirection == -1)
                    {
                        if (attackIsUpAttack)
                            rectSrcHeight = -rectSrcHeight;
                        else
                            rectSrcWidth = -rectSrcWidth;
                    }
                    Rectangle src = {0, 0, rectSrcWidth, rectSrcHeight};

                    float centerX = AttackRect.x + AttackRect.width / 2.0f;
                    float centerY = AttackRect.y + AttackRect.height / 2.0f;
                    float rectRotation = attackIsUpAttack ? -90.0f : 0.0f;
                    float drawWidth = attackIsUpAttack ? AttackRect.height : AttackRect.width;
                    float drawHeight = attackIsUpAttack ? AttackRect.width : AttackRect.height;
                    Rectangle dest = {centerX, centerY, drawWidth, drawHeight};
                    Vector2 rectOrigin = {drawWidth / 2.0f, drawHeight / 2.0f};

                    DrawTexturePro(texAttackRect[0], src, dest, rectOrigin, rectRotation, WHITE);
                }
                if (isAttacking && AttackCheck == 0)
                {
                    int rectFrame = (currentAttackFrame < 2) ? 0 : 1;
                    float rectSrcWidth = (float)texAttackRect[rectFrame].width;
                    float rectSrcHeight = (float)texAttackRect[rectFrame].height;
                    if (attackDirection == -1)
                    {
                        if (attackIsUpAttack)
                            rectSrcHeight = -rectSrcHeight;
                        else
                            rectSrcWidth = -rectSrcWidth; // flip horizontally, locked to the direction the attack started in
                    }
                    Rectangle src = {0, 0, rectSrcWidth, rectSrcHeight};

                    float centerX = AttackRect.x + AttackRect.width / 2.0f;
                    float centerY = AttackRect.y + AttackRect.height / 2.0f;
                    float rectRotation = attackIsUpAttack ? -90.0f : 0.0f;
                    float drawWidth = attackIsUpAttack ? AttackRect.height : AttackRect.width;
                    float drawHeight = attackIsUpAttack ? AttackRect.width : AttackRect.height;
                    Rectangle dest = {centerX, centerY, drawWidth, drawHeight};
                    Vector2 rectOrigin = {drawWidth / 2.0f, drawHeight / 2.0f};

                    DrawTexturePro(texAttackRect[rectFrame], src, dest, rectOrigin, rectRotation, WHITE);
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
        UnloadTexture(texAttackSide[i]);
    for (int i = 0; i < 3; i++)
        UnloadTexture(texAttackUp[i]);
    for (int i = 0; i < 2; i++)
        UnloadTexture(texAttackRect[i]);
    for (int i = 0; i < 3; i++)
        UnloadTexture(texDash[i]);

    UnloadTexture(spiritChase);
    UnloadTexture(spiritCharge);
    UnloadTexture(spiritStartBurst);
    UnloadTexture(spiritBurst);
    UnloadTexture(spiritAfterBurst);

    CloseWindow();
    return 0;
}