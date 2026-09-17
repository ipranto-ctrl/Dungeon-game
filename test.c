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
        Gameover,
        Win
    } Gamestate;
    Gamestate state = Mainmenu;
    Vector2 bootSpawn = GetLevelBottomLeftSpawn(currentLevel); // bottom-left-most safe tile of the boot level
    Player P = {
        bootSpawn.x, // x
        1200.0f,     // speed
        0.2f,        // dashtimer
        1,           // dashflag
        0.0f,        // dashcooldown
        bootSpawn.y, // y
        10000.0f,    // gravity
        0.0f,        // velocityY
        15,          // damage
        0.0f,        // attackcooldown
        100000000.0f,      // health
        100000000.0f,      // maxhealth
        .5f,         // iframes
        true,        // onground
        true,        // doublejump
        false,       // dashing
        true,        // alive
        0.0f,        // spikeknkbacktimer
        0            // spikeknkdirection
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
        0, // spiritcollision
        1  // level -- spirit spawns on level 1
    };
    // Second spirit instance dedicated to level 2's "upper platform" spirit/dragon
    // pool (see spiritsToSpawn/dragonsToSpawn below). Starts dead -- the spawner
    // brings it in and repositions/resets it each time it's its turn to spawn.
    Spirit en2 = {
        3000.0f, // x -- placeholder, repositioned on spawn
        300.0f,  // y
        400.0f,  // speed
        50.0f,   // damage
        0.0f,    // cooldown
        0.0f,    // knockbackduration
        false,   // alive -- spawner activates it
        false,
        0,
        0, // spiritcollision
        2  // level -- lives on level 2
    };
    Bull bulls[6] = {
        // Topmost platform of level 1: rows 1-3 are open air under the ceiling border,
        // row 4 is the floor, broken into 3 walkable segments by gaps/spikes. One bull
        // per segment, kept well clear of the border walls and the gaps/spikes between them.
        {1564.0f, 312.0f, 100.0f, 2500.0f, 3500.0f, 90.0f, 20.0f, 1, 15000.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1, 1, Idle, true, 1.0f, 0}, // left floor segment (cols 9-16)
        {3484.0f, 312.0f, 100.0f, 1500.0f, 3500.0f, 90.0f, 20.0f, 1, 15000.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1, 1, Idle, true, 0.5f, 0}, // middle floor segment (cols 21-34)
        {4148.0f, 312.0f, 100.0f, 4000.0f, 3500.0f, 90.0f, 20.0f, 1, 25000.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1, 1, Idle, true, 1.5f, 0}, // right floor segment (cols 38-43)
        // Level 2, base/bottom platform (row 33 -- the map's floor, spanning almost
        // the whole width with nothing in the way). Two bulls patrolling the ground
        // near where the player spawns.
        {1000.0f, 4024.0f, 100.0f, 2500.0f, 3500.0f, 90.0f, 20.0f, 1, 15000.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1, 1, Idle, true, 1.0f, 2},  // base platform, left-of-center
        {3200.0f, 4024.0f, 100.0f, 2000.0f, 3500.0f, 90.0f, 20.0f, -1, 15000.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1, 1, Idle, true, 1.0f, 2}, // base platform, further right
        // Level 2, platform 6 (row 5, cols 11-49 -- the topmost platform in the level).
        {2000.0f, 440.0f, 100.0f, 2500.0f, 3500.0f, 90.0f, 20.0f, 1, 15000.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1, 1, Idle, true, 1.0f, 2},
    };
    Mimic mimics[6] = {
        {3534.0f, 1720.0f, 0.0f, 10000.0f, 0.0f, 100.0f, 15.0f, 0.0f, 1.0f, 0.0f, 1, MIdle, true, {0}, false, 0.0f, 0.0f, 1200.0f, 1},  // level 1 -- middle-mid platform, row 15's cols 21-34 segment
        {900.0f, 1800.0f, 0.0f, 10000.0f, 0.0f, 100.0f, 15.0f, 0.0f, 1.0f, 0.0f, -1, MIdle, false, {0}, false, 0.0f, 0.0f, 800.0f, 0},  // level 0 -- disabled, user wants only the 3 bulls on level 0
        {1200.0f, 1800.0f, 0.0f, 10000.0f, 0.0f, 100.0f, 15.0f, 0.0f, 1.0f, 0.0f, 1, MIdle, false, {0}, false, 0.0f, 0.0f, 1200.0f, 0}, // level 0 -- disabled, user wants only the 3 bulls on level 0
        // Level 2, first platform above the base (row 28, cols 12-37 -- the wide
        // floor directly above the ground). Two mimics, kept clear of the wall
        // edges at col 12/37 and the spike at col 20.
        {1800.0f, 3384.0f, 0.0f, 10000.0f, 0.0f, 100.0f, 15.0f, 0.0f, 1.0f, 0.0f, 1, MIdle, true, {0}, false, 0.0f, 0.0f, 1200.0f, 2},  // left of the spike
        {3400.0f, 3384.0f, 0.0f, 10000.0f, 0.0f, 100.0f, 15.0f, 0.0f, 1.0f, 0.0f, -1, MIdle, true, {0}, false, 0.0f, 0.0f, 1200.0f, 2}, // right of the spike
        // Level 2, platform 5 (row 11, cols 11-31 -- immediate upward-left platform from platform 4).
        {3200.0f, 1208.0f, 0.0f, 10000.0f, 0.0f, 100.0f, 15.0f, 0.0f, 1.0f, 0.0f, -1, MIdle, true, {0}, false, 0.0f, 0.0f, 1200.0f, 2},
    };
    Archer archers[6] = {
        // x       y       velY  grav      spd  hp    dmg  atktimer jmptimer  dir  state  alive  onground  pKBtimer  KBdur  maxspd  arrowdmg maxatktimer level
        {3534.0f, 312.0f, 0.0f, 10000.0f, 0.0f, 80.0f, 10.0f, 2.0f, 0.0f, 1, AIdle, true, false, 0.0f, 0.0f, 400.0f, 15.0f, 1.5f, 1},   // level 1 -- top-mid platform, row 4's cols 21-34 segment
        {800.0f, 1800.0f, 0.0f, 10000.0f, 0.0f, 80.0f, 10.0f, 2.0f, 0.0f, -1, AIdle, false, false, 0.0f, 0.0f, 400.0f, 15.0f, 1.5f, 0}, // level 0 -- disabled, user wants only the 3 bulls on level 0
        {1400.0f, 1800.0f, 0.0f, 10000.0f, 0.0f, 80.0f, 10.0f, 2.0f, 0.0f, 1, AIdle, false, false, 0.0f, 0.0f, 400.0f, 15.0f, 1.5f, 0}, // level 0 -- disabled, user wants only the 3 bulls on level 0
        // Level 2, platform 3 (row 19, cols 14-32 -- immediate platform above
        // platform 2). Placed clear of the wall edges at col 14/32 and the
        // spike at col 17.
        {2700.0f, 2232.0f, 0.0f, 10000.0f, 0.0f, 80.0f, 10.0f, 2.0f, 0.0f, 1, AIdle, true, false, 0.0f, 0.0f, 400.0f, 15.0f, 1.5f, 2},
        // Level 2, platform 5 (row 11, cols 11-31 -- immediate upward-left platform from platform 4).
        {1700.0f, 1208.0f, 0.0f, 10000.0f, 0.0f, 80.0f, 10.0f, 2.0f, 0.0f, 1, AIdle, true, false, 0.0f, 0.0f, 400.0f, 15.0f, 1.5f, 2},
        // Level 2, platform 6 (row 5, cols 11-49 -- the topmost platform in the level).
        {4500.0f, 440.0f, 0.0f, 10000.0f, 0.0f, 80.0f, 10.0f, 2.0f, 0.0f, -1, AIdle, true, false, 0.0f, 0.0f, 400.0f, 15.0f, 1.5f, 2},
    };
    int archerCount = 6;            // loop covers archers[0..5]: index 0 is level 1, indices 1-2 are level-0 archers (now disabled, see alive=false above), indices 3-5 are the level 2 archers (platforms 3, 5, 6)
    Arrow arrows[MAX_ARROWS] = {0}; // zero-init means all alive=false

    Totem totems[3] = {
        // x       y       health damage atktimer maxatktimer alive knockbackduration playerecoil recoildirection level
        {2688.0f, 2744.0f, 60.0f, 10.0f, 5.0f, 1.5f, true, 0.0f, 0.0f, 0, 0}, // level 0 -- disabled, user wants only the 3 bulls on level 0
        {576.0f, 2744.0f, 60.0f, 10.0f, 5.0f, 1.5f, true, 0.0f, 0.0f, 0, 2},  // level 2 -- 2nd platform, the solid block at row 23 cols 1-8, leftward/up from the first platform
        {5300.0f, 1720.0f, 60.0f, 10.0f, 5.0f, 1.5f, true, 0.0f, 0.0f, 0, 2}, // level 2 -- 4th platform, row 15 cols 37-49, immediate upward-right platform from platform 3
    };
    int totemCount = 3;                                   // 3 totems: level 0's totem is disabled (alive=false), level 2 2nd platform, level 2 4th platform
    HomingBullet homingBullets[MAX_HOMING_BULLETS] = {0}; // zero-init means all alive=false

    int mimicCount = 6; // loop covers mimics[0..5]: index 0 is level 1 (middle-mid platform), indices 1-2 are level-0 mimics (now disabled, see alive=false above), indices 3-5 are the level 2 mimics (platforms 1 and 5)
    int mimicattaks[mimicCount];
    int bullCount = 6; // active: bulls[0..2] (level 1, topmost platform) + bulls[3..5] (level 2, base platform + platform 6)

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
        2,       // level -- dragon boss lives on level 2
    };
    // dragon.health = 500.0f;

    // Level 2 "upper platform" spirit/dragon pool -----------------------------
    // 3 spirits (en2) and 3 dragons (dragon) spawn over the course of level 2,
    // one at a time -- never both alive together, and the next one only
    // appears once the current one is destroyed. Which type goes next is
    // picked at random (see the spawner in the main loop below). These count
    // down as each one is spawned (not as they die).
    int spiritsToSpawn = 3;
    int dragonsToSpawn = 3;

    // float timer = 1; dont know what i used this for

    InitWindow(1440, 1080, "Title:The Name");

    // Spirit Texture Load
    Texture2D spiritChase = LoadTexture("Sprite/Spirit_Chase100x100.png");
    Texture2D spiritCharge = LoadTexture("Sprite/300x100_ChargeUp.png");
    Texture2D spiritStartBurst = LoadTexture("Sprite/100x100_Start2Burst.png");
    Texture2D spiritBurst = LoadTexture("Sprite/100x100_Burst.png");
    Texture2D spiritAfterBurst = LoadTexture("Sprite/300x100_AfterBurst.png");

    // Pause Menu texture Load
    Texture2D texPauseMenu = LoadTexture("img/pause_menu.png");

    // Win screen texture Load (shown after clearing the final level's gate)
    Texture2D texWin = LoadTexture("img/GameOver.png");

    // UFO Texture Load
    Texture2D texUFO = LoadTexture("img/UFO_IMG.png");

    // UFO Laser Beam Texture Load (5-frame animation played while the dragon/UFO is firing)
    Texture2D texBeam[5];
    texBeam[0] = LoadTexture("img/beam1.png");
    texBeam[1] = LoadTexture("img/beam2.png");
    texBeam[2] = LoadTexture("img/beam3.png");
    texBeam[3] = LoadTexture("img/beam4.png");
    texBeam[4] = LoadTexture("img/beam5.png");

    // Level Background Textures (one per level, indexed by currentLevel)
Texture2D texLevelBG[3];
texLevelBG[0] = LoadTexture("img/level0bg.png"); // 1248x848
texLevelBG[1] = LoadTexture("img/level1bg.png"); // 1521x1034
texLevelBG[2] = LoadTexture("img/level2bg.png"); // 1521x1034

Texture2D texTile[3];
    texTile[0] = LoadTexture("img/level0tile.png"); 
    texTile[1] = LoadTexture("img/level1tile.png"); 
    texTile[2] = LoadTexture("img/level2tile.png");
    SetExitKey(KEY_DELETE);
    HideCursor();
    ToggleFullscreen();
    int screen_h = GetScreenHeight();
    int screen_w = GetScreenWidth();
    SetTargetFPS(60);

    // Load Textures
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

    // Bull Textures
    Texture2D texBullIdle = LoadTexture("img/bullidle.png");

    Texture2D texBullRun[4];
    texBullRun[0] = LoadTexture("img/bullrun1.png");
    texBullRun[1] = LoadTexture("img/bullrun2.png");
    texBullRun[2] = LoadTexture("img/bullrun3.png");
    texBullRun[3] = LoadTexture("img/bullrun4.png");

    Texture2D texBullStop[2];
    texBullStop[0] = LoadTexture("img/bullstop1.png");
    texBullStop[1] = LoadTexture("img/bullstop2.png");

    // Bull Animation Variables
    float bullAnimTimer = 0.0f;
    int currentBullRunFrame = 0;
    int currentBullStopFrame = 0;

    // Load Mimic Textures
    Texture2D texMimicIdle = LoadTexture("img/mimicidle.png");

    Texture2D texMimicRun[2];
    texMimicRun[0] = LoadTexture("img/mimicrun1.png");
    texMimicRun[1] = LoadTexture("img/mimicrun2.png");

    // Mimic Attack Textures -- charging still uses the idle texture as a placeholder
    // until a dedicated charge-frame asset exists
    Texture2D texMimicAttack[2];
    texMimicAttack[0] = LoadTexture("img/mimicattack1.png");      // strike release
    texMimicAttack[1] = LoadTexture("img/mimicattack2.png");      // recovery back toward idle
    Texture2D texMimicCharge = LoadTexture("img/mimicharge.png"); // charging wind-up pose

    // Mimic attack-impact particle burst -- square canvas, replaces the YELLOW
    // attackrect placeholder. Plays once the swing animation finishes rather than
    // stretching into the hitbox shape, same idea as the double-jump burst frames.
    Texture2D texMimicParticle[2];
    texMimicParticle[0] = LoadTexture("img/mimicparticle1.png"); // burst begins
    texMimicParticle[1] = LoadTexture("img/mimicparticle2.png"); // fade out

    // Mimic Animation Variables
    float mimicAnimTimer = 0.0f;
    int currentMimicRunFrame = 0;
    float mimicWalkCycleTimer = 0.0f; // continuous (never resets) -- drives the bob/lean offset independent of frame-swap timing

    // Per-mimic attack animation timers (resized to 6 to match the mimics[6] array elsewhere)
    float mimicAttackAnimTimer[6] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
    bool mimicAttackAnimActive[6] = {false, false, false, false, false, false}; // latched separately from mimicattaks[i], which may only pulse true for a single frame
    const float MIMIC_ATTACK_ANIM_DURATION = 0.4f;                              // total time to play through both attack frames -- bumped up so the swing is actually visible

    // Per-mimic attack-impact particle burst -- fires on the falling edge of
    // mimicAttackAnimActive[i] (i.e. once the swing finishes), at wherever
    // mimics[i].attackrect last was. attackrect itself is only valid on the single
    // frame mimicattaks[i] pulses true, so it has to be snapshotted then and reused
    // once the anim ends and the real attackrect may already be stale/zeroed.
    float mimicParticleTimer[6] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
    Rectangle mimicParticleRect[6] = {0};       // snapshot of attackrect from the last valid hit-check frame
    const float MIMIC_PARTICLE_DURATION = 0.2f; // total time to play through both particle frames
    const float MIMIC_PARTICLE_SIZE = 260.0f;   // draw size (square) -- independent of attackrect's own dimensions, tune to taste

    // Per-mimic hit-flash tracking -- detects a health drop frame-to-frame (rather than
    // depending on any knockback/iframe internals inside enemies.c) and tints the sprite
    // red for a short window when it happens, same idea as the player's iframes blink.
    float mimicPrevHealth[6] = {100.0f, 100.0f, 100.0f, 100.0f, 100.0f, 100.0f}; // matches each mimic's starting health above
    float mimicHitFlashTimer[6] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
    const float MIMIC_HIT_FLASH_DURATION = 0.15f; // tune: how long the red tint holds after a hit

    // Same hit-flash pattern applied to every other enemy type -- pull the
    // starting value straight from each enemy's own struct/array instead of
    // re-typing the numbers, so this can't drift out of sync if those change.
    float bullPrevHealth[6] = {bulls[0].health, bulls[1].health, bulls[2].health, bulls[3].health, bulls[4].health, bulls[5].health};
    float bullHitFlashTimer[6] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
    const float BULL_HIT_FLASH_DURATION = 0.15f;

    float archerPrevHealth[6] = {archers[0].health, archers[1].health, archers[2].health, archers[3].health, archers[4].health, archers[5].health};
    float archerHitFlashTimer[6] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
    const float ARCHER_HIT_FLASH_DURATION = 0.15f;

    float totemPrevHealth[3] = {totems[0].health, totems[1].health, totems[2].health};
    float totemHitFlashTimer[3] = {0.0f, 0.0f, 0.0f};
    const float TOTEM_HIT_FLASH_DURATION = 0.15f;

    float dragonPrevHealth = dragon.health;
    float dragonHitFlashTimer = 0.0f;
    const float DRAGON_HIT_FLASH_DURATION = 0.15f;

    // UFO Laser Beam Animation Variables -- cycles through the 5 beam frames
    // while the dragon/UFO is actively firing (dstate == Dattacking), looping
    // for as long as the attack lasts.
    float beamAnimTimer = 0.0f;
    int currentBeamFrame = 0;
    const float BEAM_FRAME_DURATION = 0.06f; // time each beam frame is shown; tune to taste

    // Animation Variables
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
    float attackCooldownAtTrigger = 0.0f; // cooldown value captured the instant this swing started; kept so we know when the real hitbox/cooldown window has ended
    // NEW: attack animation now runs off its own short timer instead of the (often much longer)
    // attackcooldown value, so the swing sprite reads fast/punchy regardless of how long the
    // cooldown before the next attack actually is.
    float attackAnimTimer = 0.0f;
    const float ATTACK_ANIM_DURATION = .1f; // total time to play through all 3 attack frames; lower = snappier
    int attackDirection = 1;                // facing direction locked in at the moment the attack starts; used for AttackRect sprite flip only
    bool attackIsUpAttack = false;          // whether the current swing is the up-attack, locked in at the moment the attack starts
    int currentDashFrame = 0;
    float dashDurationAtTrigger = 0.0f; // P.dashtimer value captured the instant the dash starts; drives animation progress the same way attackCooldownAtTrigger does
    float dashParticleX = 0.0f;         // world-space launch point captured at dash start, so the burst stays put while the player rockets away from it
    float dashParticleY = 0.0f;

    // Prevents the gate from re-triggering every single frame while the
    // player is still standing inside it (CheckGateCollision would otherwise
    // fire again the instant they land at the new level's spawn point).
    float gateCooldown = 0.0f;

    // Archer Textures
    Texture2D texArcherIdle = LoadTexture("img/oldidle.png");

    Texture2D texArcherWalk[2];
    texArcherWalk[0] = LoadTexture("img/oldwalk1.png");
    texArcherWalk[1] = LoadTexture("img/oldwalk2.png");

    Texture2D texArcherAttack[4];
    texArcherAttack[0] = LoadTexture("img/oldthrow1.png");
    texArcherAttack[1] = LoadTexture("img/oldthrow2.png");
    texArcherAttack[2] = LoadTexture("img/oldthrow3.png");
    texArcherAttack[3] = LoadTexture("img/oldthrow4.png");

    Texture2D texArcherSpawn[2]; // Used as the reload/respawn arrow animation
    texArcherSpawn[0] = LoadTexture("img/oldspawn1.png");
    texArcherSpawn[1] = LoadTexture("img/oldspawn2.png");

    Texture2D texCake = LoadTexture("img/cake.png");

    // Archer Animation Variables
    float archerAnimTimerWalk = 0.0f;
    int currentArcherWalkFrame = 0;

    // Per-archer visual state tracking
    float archerSpawnTimer[6] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
    float archerPrevAttackTimer[6] = {archers[0].attacktimer, archers[1].attacktimer, archers[2].attacktimer, archers[3].attacktimer, archers[4].attacktimer, archers[5].attacktimer};
    float archerPrevX[6] = {archers[0].x, archers[1].x, archers[2].x, archers[3].x, archers[4].x, archers[5].x};
    // --- Load Totem Textures ---
    Texture2D texTotem[4];
    texTotem[0] = LoadTexture("img/totem1.png");
    texTotem[1] = LoadTexture("img/totem2.png");
    texTotem[2] = LoadTexture("img/totem3.png");
    texTotem[3] = LoadTexture("img/totem4.png");

    // Totem Animation Variables
    float totemAnimTimer = 0.0f;
    int currentTotemFrame = 0;

    // --- Load Spike Textures ---
    Texture2D texSpike[2];
    texSpike[0] = LoadTexture("img/spike1.png");
    texSpike[1] = LoadTexture("img/spike2.png");

    // Spike Animation Variables
    float spikeAnimTimer = 0.0f;
    int currentSpikeFrame = 0;

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
            DrawText("LDF PRESENTS", screen_w / 2 - 400, screen_h / 2 - 300, 100, RED);
            DrawText("UNTITLED DUNGEON CRAWLER", screen_w / 2 - 750, screen_h / 2 + 100, 100, RED);
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
            Rectangle pauseSrc = {0, 0, (float)texPauseMenu.width, (float)texPauseMenu.height}; // starts from 0,0 pixel from the main image
            Rectangle pauseDest = {0, 0, (float)screen_w, (float)screen_h};                     // where, and how big, to draw it on screen, describes rectangle on screen
            DrawTexturePro(texPauseMenu, pauseSrc, pauseDest, (Vector2){0, 0}, 0.0f, WHITE);    // 0.0f means no rotation
            EndDrawing();
        }
        if (state == Playing)
        {
            if (IsKeyPressed(KEY_ESCAPE))
            {
                state = Pausemenu;
                BeginDrawing();
                ClearBackground(BLACK);
                EndDrawing();
            }

            // Gate is always open (doorOpen stays true) -- no enemies-dead
            // gating for now. If that comes back later, set doorOpen here
            // instead of leaving it permanently true in tilemap.c.

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
                // --- Update Spike Animation Timer ---
                spikeAnimTimer += dt;
                if (spikeAnimTimer >= 0.03f) // Switch frames every 0.5 seconds
                {
                    currentSpikeFrame = (currentSpikeFrame + 1) % 2;
                    spikeAnimTimer = 0.0f;
                }
                // --- Update Archer Animation Timer ---
                archerAnimTimerWalk += dt;
                if (archerAnimTimerWalk >= 0.15f)
                {
                    currentArcherWalkFrame = (currentArcherWalkFrame + 1) % 2;
                    archerAnimTimerWalk = 0.0f;
                }

                // --- Update Bull Animation Timers ---
                bullAnimTimer += dt;
                if (bullAnimTimer >= 0.1f) // Switches frame every 0.1 seconds (~10 FPS animation)
                {
                    currentBullRunFrame = (currentBullRunFrame + 1) % 4;
                    currentBullStopFrame = (currentBullStopFrame + 1) % 2;
                    bullAnimTimer = 0.0f;
                }

                // --- Update Mimic Animation Timer ---
                mimicAnimTimer += dt;
                if (mimicAnimTimer >= 0.15f) // slower than the Bull's 0.1s -- he's on crutches, not sprinting
                {
                    currentMimicRunFrame = (currentMimicRunFrame + 1) % 2;
                    mimicAnimTimer = 0.0f;
                }
                // Separate continuous accumulator (never resets) for the bob/lean offset below,
                // so that motion stays smooth regardless of the 2-frame swap timing above.
                mimicWalkCycleTimer += dt;
                // --- Update Totem Animation Timer ---
                totemAnimTimer += dt;
                if (totemAnimTimer >= 0.15f) // Switch frames every 0.15 seconds
                {
                    currentTotemFrame = (currentTotemFrame + 1) % 4;
                    totemAnimTimer = 0.0f;
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

                // --- Gate / level transition ---
                if (gateCooldown > 0.0f)
                    gateCooldown -= dt;
                else if (CheckGateCollision(&P))
                {
                    if (currentLevel == LEVEL_COUNT - 1)
                    {
                        // Gate on the final level -- the run is complete instead
                        // of looping back to level 0.
                        state = Win;
                    }
                    else
                    {
                        currentLevel = (currentLevel + 1) % LEVEL_COUNT;
                        Vector2 spawn = GetLevelBottomLeftSpawn(currentLevel);
                        P.x = spawn.x;
                        P.y = spawn.y;
                        P.velocityY = 0.0f;
                        P.onground = false;
                        P.doublejump = true;
                        P.dashing = false;
                        P.dashtimer = 0.15f;
                        gateCooldown = 0.5f; // long enough to clear the gate tile before re-checking
                    }
                }

                AttackCheck = UpdateAttack(&P, dt, &AttackRect);

                for (int i = 0; i < bullCount; i++)
                {
                    BullCollisionX(&bulls[i]);

                    UpdateBullGravity(&bulls[i], dt);

                    BullCollisionY(&bulls[i]);

                    BullUpdateLogic(&bulls[i], &P, dt, AttackCheck, &AttackRect);

                    // Hit-flash: same "did health just drop" detection as the mimics.
                    if (bulls[i].health < bullPrevHealth[i])
                    {
                        bullHitFlashTimer[i] = BULL_HIT_FLASH_DURATION;
                    }
                    bullPrevHealth[i] = bulls[i].health;
                    if (bullHitFlashTimer[i] > 0.0f)
                    {
                        bullHitFlashTimer[i] -= dt;
                    }

                    CollisionX(&P);
                }

                for (int i = 0; i < mimicCount; i++)
                {
                    MimicCollisionX(&mimics[i]);
                    UpdateMimicGravity(&mimics[i], dt);
                    MimicCollisionY(&mimics[i]);
                    mimicattaks[i] = UpdateMimicLogic(&mimics[i], &P, dt, AttackCheck, &AttackRect);

                    // mimicattaks[i] may only pulse true for a single frame (the actual hit
                    // check), which was making the attack sprite flash on for one frame and
                    // vanish. Latch a separate "is the attack animation playing" flag on the
                    // rising edge and let it run its own fixed duration regardless of whether
                    // the raw trigger flag stays true.
                    bool wasMimicAttackAnimActive = mimicAttackAnimActive[i]; // captured before this frame's update, to detect the swing starting below

                    if (mimicattaks[i] && !mimicAttackAnimActive[i])
                    {
                        mimicAttackAnimActive[i] = true;
                        mimicAttackAnimTimer[i] = 0.0f;
                    }
                    if (mimicAttackAnimActive[i])
                    {
                        mimicAttackAnimTimer[i] += dt;
                        if (mimicAttackAnimTimer[i] >= MIMIC_ATTACK_ANIM_DURATION)
                        {
                            mimicAttackAnimActive[i] = false;
                            mimicAttackAnimTimer[i] = 0.0f;
                        }
                    }

                    // attackrect is only meaningful on the single frame mimicattaks[i]
                    // pulses true (the real hit-check) -- which is the same frame the
                    // swing starts, so it's already valid at the moment we need it below.
                    if (mimicattaks[i])
                    {
                        mimicParticleRect[i] = mimics[i].attackrect;
                    }

                    // Fire the impact burst the instant the swing starts (false -> true
                    // edge of mimicAttackAnimActive), not when it ends -- reads as the
                    // impact happening right as the attack lands.
                    if (!wasMimicAttackAnimActive && mimicAttackAnimActive[i])
                    {
                        mimicParticleTimer[i] = MIMIC_PARTICLE_DURATION;
                    }
                    if (mimicParticleTimer[i] > 0.0f)
                    {
                        mimicParticleTimer[i] -= dt;
                    }

                    // Hit-flash: trigger on any frame where health just dropped, then count
                    // down independent of what caused the drop.
                    if (mimics[i].health < mimicPrevHealth[i])
                    {
                        mimicHitFlashTimer[i] = MIMIC_HIT_FLASH_DURATION;
                    }
                    mimicPrevHealth[i] = mimics[i].health;
                    if (mimicHitFlashTimer[i] > 0.0f)
                    {
                        mimicHitFlashTimer[i] -= dt;
                    }
                }

                for (int i = 0; i < archerCount; i++)
                {
                    ArcherCollisionX(&archers[i]);
                    UpdateArcherGravity(&archers[i], dt);
                    ArcherCollisionY(&archers[i]);
                    UpdateArcherLogic(&archers[i], &P, dt, AttackCheck, &AttackRect, arrows, MAX_ARROWS);

                    if (archers[i].health < archerPrevHealth[i])
                    {
                        archerHitFlashTimer[i] = ARCHER_HIT_FLASH_DURATION;
                    }
                    archerPrevHealth[i] = archers[i].health;
                    if (archerHitFlashTimer[i] > 0.0f)
                    {
                        archerHitFlashTimer[i] -= dt;
                    }
                }

                bool wasDragonAttacking = (dragon.dstate == Dattacking); // captured before this frame's update, to detect the beam starting below

                DragonCollisionX(&dragon, dt);
                DragonCollisionY(&dragon);
                UpdateDragon(&dragon, &P, dt, AttackCheck, &AttackRect);

                // --- Update UFO Laser Beam Animation ---
                // Restart the cycle on the rising edge (attack just started), then
                // loop through the 5 frames for as long as the dragon stays in
                // Dattacking so the beam reads as a continuous laser.
                if (dragon.dstate == Dattacking)
                {
                    if (!wasDragonAttacking)
                    {
                        beamAnimTimer = 0.0f;
                        currentBeamFrame = 0;
                    }
                    beamAnimTimer += dt;
                    if (beamAnimTimer >= BEAM_FRAME_DURATION)
                    {
                        currentBeamFrame = (currentBeamFrame + 1) % 5;
                        beamAnimTimer = 0.0f;
                    }
                }
                else
                {
                    beamAnimTimer = 0.0f;
                    currentBeamFrame = 0;
                }

                if (dragon.health < dragonPrevHealth)
                {
                    dragonHitFlashTimer = DRAGON_HIT_FLASH_DURATION;
                }
                dragonPrevHealth = dragon.health;
                if (dragonHitFlashTimer > 0.0f)
                {
                    dragonHitFlashTimer -= dt;
                }

                UpdateArrows(arrows, MAX_ARROWS, &P, dt);

                for (int i = 0; i < totemCount; i++)
                {
                    TotemCollision(&totems[i], &P);
                    UpdateTotemLogic(&totems[i], &P, dt, AttackCheck, &AttackRect, homingBullets, MAX_HOMING_BULLETS);

                    if (totems[i].health < totemPrevHealth[i])
                    {
                        totemHitFlashTimer[i] = TOTEM_HIT_FLASH_DURATION;
                    }
                    totemPrevHealth[i] = totems[i].health;
                    if (totemHitFlashTimer[i] > 0.0f)
                    {
                        totemHitFlashTimer[i] -= dt;
                    }
                }
                UpdateHomingBullets(homingBullets, MAX_HOMING_BULLETS, &P, dt, AttackCheck, &AttackRect);

                spiritupdate(&en, &P, dt);
                spiritupdate(&en2, &P, dt);

                //  Level 2 "upper platform" spirit/dragon spawner
                // Only relevant on level 2, and only when nothing from this pool is
                // currently alive. Picks a type at random (forced to whichever type
                // still has some left, if only one does), and resets its stats fresh.
                // Spirit spawns in the open air around the topmost platform; dragon
                // spawns lower down, under platform 4, in the
                // open air of rows not right up on the topmost platform.
                if (currentLevel == 2 && !en2.alive && !dragon.alive && (spiritsToSpawn > 0 || dragonsToSpawn > 0))
                {
                    bool spawnSpirit;
                    if (spiritsToSpawn <= 0)
                        spawnSpirit = false;
                    else if (dragonsToSpawn <= 0)
                        spawnSpirit = true;
                    else
                        spawnSpirit = (GetRandomValue(0, 1) == 0);

                    if (spawnSpirit)
                    {
                        en2.x = (float)GetRandomValue(1600, 4400);
                        en2.y = 300.0f; // open air above/around the topmost platform (row 5)
                        en2.speed = 400.0f;
                        en2.damage = 50.0f;
                        en2.cooldown = 0.0f;
                        en2.knockbackduration = 0.0f;
                        en2.spiritcollision = false;
                        en2.alive = true;
                        spiritsToSpawn--;
                    }
                    else
                    {
                        dragon.x = (float)GetRandomValue(4900, 6100); // roughly under platform 4's own span (cols 37-49)
                        dragon.y = 2100.0f;                           // rows 16-17, the open air just below platform 4's floor
                        dragon.health = 50.0f;
                        dragon.chargetimer = 0.0f;
                        dragon.attacktimer = 0.0f;
                        dragon.dstate = Didle;
                        dragon.direction = 1;
                        dragon.knockbackduration = 0.0f;
                        dragon.playerknockbacktimer = 0.0f;
                        dragon.playerecoil = 0.0f;
                        dragon.recoildirection = 0;
                        dragon.wallDropSpeed = 0.0f;
                        dragon.alive = true;
                        dragonPrevHealth = dragon.health;
                        dragonsToSpawn--;
                    }
                }

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
                // --- Draw level background (world-space, so it pans/zooms with camera) ---
{
    Texture2D bgTex = texLevelBG[currentLevel];
    Rectangle bgSrc = {0.0f, 0.0f, (float)bgTex.width, (float)bgTex.height};
    Rectangle bgDest = {0.0f, 0.0f, (float)(MAP_COLS * TILE_SIZE), (float)(MAP_ROWS * TILE_SIZE)};
    DrawTexturePro(bgTex, bgSrc, bgDest, (Vector2){0.0f, 0.0f}, 0.0f, WHITE);
}
                // --- NEW: Determine which texture to draw ---
                Texture2D currentTex = texIdle;

                // if (AttackCheck)
                //     DrawRectangleRec(AttackRect, RED);  true attackhitbox

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
                    attackAnimTimer = 0.0f;                     // NEW: restart the animation clock for this swing
                    attackDirection = P.dashflag;               // lock in facing direction for the whole swing, set only on the trigger frame
                    attackIsUpAttack = IsKeyDown(KEY_W);        // same condition UpdateAttack used internally to build the up-attack AttackRect
                }
                if (isAttacking)
                {
                    // NEW: Derive animation progress from its own short fixed-length timer
                    // (ATTACK_ANIM_DURATION) instead of the real attack cooldown, so the swing
                    // sprite always plays at the same snappy speed no matter how long the
                    // cooldown/hitbox window happens to be. The sprite plays through once and
                    // then holds its last frame until the real cooldown (and thus isAttacking)
                    // ends, so hit-detection timing is completely unaffected.
                    attackAnimTimer += dt;
                    float progress = (ATTACK_ANIM_DURATION > 0.0f)
                                         ? (attackAnimTimer / ATTACK_ANIM_DURATION)
                                         : 1.0f;
                    if (progress > 1.0f)
                        progress = 1.0f;

                    currentAttackFrame = (int)(progress * 3.0f);
                    if (currentAttackFrame > 2)
                        currentAttackFrame = 2;
                    if (currentAttackFrame < 0)
                        currentAttackFrame = 0;

                    if (attackAnimTimer >= ATTACK_ANIM_DURATION)
                        isAttacking = false; // animation itself decides when the swing sprite ends -- attackcooldown still separately gates when you can attack again

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

                // Spirit Sprites (drawn after the player so it renders on top)
                if (en.alive == true && en.level == currentLevel)
                {
                    Texture2D currentSpiritTex = spiritChase;
                    int frames = 1;
                    int currentFrame = 0;
                    float scaleSize = 100.0f;     // Baseline Dimension Scaling
                    float spiritOffsetX = -25.0f; //(hitbox 50/2) - (scaleSize 100/2)
                    float spiritOffsetY = -25.0f;

                    if (en.spiritcollision == true && en.knockbackduration <= 0)
                    {
                        // Phase 2: Charging up (en.cooldown ticks down from 0.5 to 0)
                        if (en.cooldown > 0.1f)
                        {
                            currentSpiritTex = spiritCharge;
                            frames = 3;
                            float timeElapsed = 0.5f - en.cooldown; // Ranges from 0.0 to 0.4
                            currentFrame = (int)(timeElapsed / (0.4f / 3.0f));
                            if (currentFrame > 2)
                                currentFrame = 2;
                        }
                        else
                        {
                            currentSpiritTex = spiritStartBurst;
                            frames = 1;
                            currentFrame = 0;
                            scaleSize = 300.0f;      // Scale up for tension,, burst boro choto hoy
                            spiritOffsetX = -125.0f; // (50/2) - (300/2)
                            spiritOffsetY = -125.0f;
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
                            scaleSize = 400.0f;      // Explosion expands past hitbox edges
                            spiritOffsetX = -175.0f; // (50/2) - (400/2)
                            spiritOffsetY = -175.0f;
                        }
                        else
                        {
                            currentSpiritTex = spiritAfterBurst;
                            frames = 3;
                            float timeElapsed = 0.2f - en.knockbackduration; // Ranges from 0.0 to 0.2
                            currentFrame = (int)(timeElapsed / (0.2f / 3.0f));
                            if (currentFrame > 2)
                                currentFrame = 2;
                            scaleSize = 400.0f;
                            spiritOffsetX = -175.0f;
                            spiritOffsetY = -175.0f;
                        }
                    }

                    float frameWidth = (float)currentSpiritTex.width / frames;
                    Rectangle src = {currentFrame * frameWidth, 0, frameWidth, (float)currentSpiritTex.height};

                    // Flip texture based on direction if it's currently chasing
                    if (!en.spiritcollision && en.x > P.x)
                        src.width = -src.width;

                    Rectangle dest = {en.x + spiritOffsetX, en.y + spiritOffsetY, scaleSize, scaleSize};
                    Vector2 spiritOrigin = {0, 0};
                    DrawTexturePro(currentSpiritTex, src, dest, spiritOrigin, 0.0f, WHITE);
                }

                // Second spirit instance (en2) -- level 2's upper-platform pool.
                // Same draw logic as en above, just driven off en2's own state.
                if (en2.alive == true && en2.level == currentLevel)
                {
                    Texture2D currentSpiritTex2 = spiritChase;
                    int frames2 = 1;
                    int currentFrame2 = 0;
                    float scaleSize2 = 100.0f;
                    float spiritOffsetX2 = -25.0f;
                    float spiritOffsetY2 = -25.0f;

                    if (en2.spiritcollision == true && en2.knockbackduration <= 0)
                    {
                        if (en2.cooldown > 0.1f)
                        {
                            currentSpiritTex2 = spiritCharge;
                            frames2 = 3;
                            float timeElapsed2 = 0.5f - en2.cooldown;
                            currentFrame2 = (int)(timeElapsed2 / (0.4f / 3.0f));
                            if (currentFrame2 > 2)
                                currentFrame2 = 2;
                        }
                        else
                        {
                            currentSpiritTex2 = spiritStartBurst;
                            frames2 = 1;
                            currentFrame2 = 0;
                            scaleSize2 = 300.0f;
                            spiritOffsetX2 = -125.0f;
                            spiritOffsetY2 = -125.0f;
                        }
                    }
                    else if (en2.knockbackduration > 0)
                    {
                        if (en2.knockbackduration > 0.2f)
                        {
                            currentSpiritTex2 = spiritBurst;
                            frames2 = 1;
                            currentFrame2 = 0;
                            scaleSize2 = 400.0f;
                            spiritOffsetX2 = -175.0f;
                            spiritOffsetY2 = -175.0f;
                        }
                        else
                        {
                            currentSpiritTex2 = spiritAfterBurst;
                            frames2 = 3;
                            float timeElapsed2 = 0.2f - en2.knockbackduration;
                            currentFrame2 = (int)(timeElapsed2 / (0.2f / 3.0f));
                            if (currentFrame2 > 2)
                                currentFrame2 = 2;
                            scaleSize2 = 400.0f;
                            spiritOffsetX2 = -175.0f;
                            spiritOffsetY2 = -175.0f;
                        }
                    }

                    float frameWidth2 = (float)currentSpiritTex2.width / frames2;
                    Rectangle src2 = {currentFrame2 * frameWidth2, 0, frameWidth2, (float)currentSpiritTex2.height};

                    if (!en2.spiritcollision && en2.x > P.x)
                        src2.width = -src2.width;

                    Rectangle dest2 = {en2.x + spiritOffsetX2, en2.y + spiritOffsetY2, scaleSize2, scaleSize2};
                    Vector2 spiritOrigin2 = {0, 0};
                    DrawTexturePro(currentSpiritTex2, src2, dest2, spiritOrigin2, 0.0f, WHITE);
                }

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
                    if (bulls[i].alive && bulls[i].level == currentLevel)
                    {
                        Texture2D bullTex = texBullIdle;

                        if (bulls[i].state == Charging)
                        {
                            bullTex = texBullRun[currentBullRunFrame];
                        }
                        else if (bulls[i].state == Stopping)
                        {
                            bullTex = texBullStop[currentBullStopFrame];
                        }
                        else
                        {
                            bullTex = texBullIdle;
                        }

                        float sourceWidth = (float)bullTex.width;
                        if (bulls[i].direction == -1)
                        {
                            sourceWidth = -sourceWidth;
                        }

                        Rectangle sourceRec = {0.0f, 0.0f, sourceWidth, (float)bullTex.height};

                        // Preserve each texture's own aspect ratio instead of forcing a fixed
                        // 250x250 square -- stop frames aren't the same proportions as idle/run,
                        // so a hardcoded square squishes/"slims" them.
                        float bullAspect = (float)bullTex.width / (float)bullTex.height;
                        float maxBullHeight = 250.0f; // tune to taste
                        float bullDrawHeight = maxBullHeight;
                        float bullDrawWidth = bullAspect * maxBullHeight;

                        // Center horizontally over the 200px hitbox, feet aligned to the bottom
                        float offsetX = (bullDrawWidth - 200.0f) / 2.0f;
                        float offsetY = bullDrawHeight - 200.0f;

                        Rectangle destRec = {
                            bulls[i].x - offsetX,
                            bulls[i].y - offsetY,
                            bullDrawWidth,
                            bullDrawHeight};

                        Vector2 origin = {0.0f, 0.0f};

                        Color bullTint = (bullHitFlashTimer[i] > 0.0f) ? RED : WHITE;

                        DrawTexturePro(bullTex, sourceRec, destRec, origin, 0.0f, bullTint);
                    }
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
                        {
                            // Check if the current tile is on the extreme edges of the map
                            bool isBorder = (i == 0 || i == MAP_ROWS - 1 || j == 0 || j == MAP_COLS - 1);

                            if (isBorder)
                            {
                                // Draw solid black for the borders
                                DrawRectangle((j * TILE_SIZE), (i * TILE_SIZE), TILE_SIZE, TILE_SIZE, BLACK);
                            }
                            else
                            {
                                // Draw the resized level texture for inner platforms
                                Texture2D currentTileTex = texTile[currentLevel];
                                Rectangle tileSrc = {0.0f, 0.0f, (float)currentTileTex.width, (float)currentTileTex.height};
                                Rectangle tileDest = {(float)(j * TILE_SIZE), (float)(i * TILE_SIZE), (float)TILE_SIZE, (float)TILE_SIZE};
                                DrawTexturePro(currentTileTex, tileSrc, tileDest, (Vector2){0, 0}, 0.0f, WHITE);
                            }
                        }
                        if (maps[currentLevel][i][j] == 3)
                        // DrawRectangle((j * TILE_SIZE), (i * TILE_SIZE), TILE_SIZE, TILE_SIZE, ORANGE); // spike
                        // // if (maps[currentLevel][i][j] == 3)
                        {
                            Texture2D currentSpikeTex = texSpike[currentSpikeFrame];
                            Rectangle spikeSrc = {0.0f, 0.0f, (float)currentSpikeTex.width, (float)currentSpikeTex.height};
                            Rectangle spikeDest = {(j * TILE_SIZE), (i * TILE_SIZE), TILE_SIZE, TILE_SIZE};
                            DrawTexturePro(currentSpikeTex, spikeSrc, spikeDest, (Vector2){0, 0}, 0.0f, WHITE);
                        }
                        if (maps[currentLevel][i][j] == 2) // door
                        {
                            if (doorOpen)
                                DrawRectangleLines((j * TILE_SIZE), (i * TILE_SIZE), TILE_SIZE, TILE_SIZE, GREEN); // open: just an outline, fully walkable
                            else
                                DrawRectangle((j * TILE_SIZE), (i * TILE_SIZE), TILE_SIZE, TILE_SIZE, BROWN); // closed: solid gate
                        }
                    }
                }
                // --- Draw Mimic Enemies ---
                for (int i = 0; i < mimicCount; i++)
                {
                    if (mimics[i].alive && mimics[i].level == currentLevel)
                    {
                        Texture2D mimicTex = texMimicIdle;
                        bool mimicIsWalking = false;

                        // Attack pose takes priority over the state-based texture whenever
                        // this mimic's attack flag is active, cycling strike -> recovery off
                        // its own per-instance timer. Falls back to state-based texture (idle
                        // /run) once the attack flag clears.
                        if (mimicAttackAnimActive[i])
                        {
                            float attackProgress = mimicAttackAnimTimer[i] / MIMIC_ATTACK_ANIM_DURATION;
                            if (attackProgress > 1.0f)
                                attackProgress = 1.0f;
                            mimicTex = (attackProgress < 0.5f) ? texMimicAttack[0] : texMimicAttack[1];
                        }
                        // Map Mimic's current state to the correct sprite
                        else if (mimics[i].mstate == MChasing)
                        {
                            mimicTex = texMimicRun[currentMimicRunFrame];
                            mimicIsWalking = true;
                        }
                        else // MIdle uses idle
                        {
                            mimicTex = (mimics[i].mstate == MCharging) ? texMimicCharge : texMimicIdle;
                        }

                        // Handle Direction & Horizontally Flip Texture
                        float sourceWidth = (float)mimicTex.width;
                        if (mimics[i].direction == -1)
                        {
                            sourceWidth = -sourceWidth;
                        }

                        Rectangle sourceRec = {0.0f, 0.0f, sourceWidth, (float)mimicTex.height};

                        // Scale uniformly off the texture's OWN real aspect ratio instead of
                        // assuming every frame shares the idle frame's exact 720x1456 canvas.
                        // Generated walk frames have come back at different canvas proportions
                        // (e.g. 1024x1536), and a fixed destWidth/destHeight stretch squashes
                        // those non-uniformly (reads as the sprite going "slim"). Locking to a
                        // target height and deriving width from the texture's actual dimensions
                        // keeps every frame correctly proportioned no matter its source canvas.
                        // Fit the texture into a bounded box instead of locking height alone.
                        // The idle/walk frames are tall portrait canvases (~720x1377) where
                        // height-locking works fine, but the attack frames came back as
                        // landscape canvases (1536x1024) -- height-locking those blew the
                        // render width out to ~640px (the lunge reach), which read as the
                        // sprite "growing" during attacks. Capping width too keeps that in
                        // check, but the 1.3x cosmetic width boost (kept for the portrait
                        // idle/walk frames) must NOT be applied when computing the
                        // width-constrained branch's height, or it over-shrinks it.
                        float mimicRawAspect = (float)mimicTex.width / (float)mimicTex.height;
                        float maxMimicHeight = 330.0f; // 1.5x the 200px hitbox height
                        float maxMimicWidth = 450.0f;  // tune: how wide a lunging/reaching attack pose is allowed to render
                        float mimicDrawHeight, mimicDrawWidth;
                        if (mimicRawAspect * 1.3f * maxMimicHeight <= maxMimicWidth)
                        {
                            // height-constrained: tall/narrow textures like idle and walk.
                            // *1.3 preserves the existing idle/walk proportions.
                            mimicDrawHeight = maxMimicHeight;
                            mimicDrawWidth = mimicRawAspect * 1.3f * maxMimicHeight;
                        }
                        else
                        {
                            // width-constrained: wide/landscape textures like the attack frames.
                            // No 1.3x boost here -- it was making the height shrink much more
                            // than intended for this branch.
                            mimicDrawWidth = maxMimicWidth;
                            mimicDrawHeight = maxMimicWidth / mimicRawAspect;
                        }

                        // Bob/lean offset -- only while walking, synced to the continuous
                        // mimicWalkCycleTimer (independent of the 2-frame swap timer) so the
                        // sway stays smooth regardless of how many real frames exist.
                        float mimicBobOffset = 0.0f;
                        float mimicLeanOffset = 0.0f;
                        if (mimicIsWalking)
                        {
                            float walkCycleDuration = 0.6f; // full bob/lean cycle length, tune to taste
                            float cyclePos = fmodf(mimicWalkCycleTimer, walkCycleDuration) / walkCycleDuration;
                            mimicBobOffset = sinf(cyclePos * 2.0f * PI * 2.0f) * 4.0f; // 2 bobs per cycle (double-crutch gait)
                            mimicLeanOffset = sinf(cyclePos * 2.0f * PI) * 6.0f;       // forward/back sway
                            if (mimics[i].direction == -1)
                                mimicLeanOffset = -mimicLeanOffset; // sway follows facing direction
                        }

                        // Center horizontally over the 100px hitbox, align feet to the bottom
                        float mimicOffsetX = (mimicDrawWidth - 100.0f) / 2.0f;
                        float mimicOffsetY = mimicDrawHeight - 200.0f;

                        Rectangle destRec = {
                            mimics[i].x - mimicOffsetX + mimicLeanOffset,
                            mimics[i].y - mimicOffsetY + mimicBobOffset,
                            mimicDrawWidth,
                            mimicDrawHeight};

                        Vector2 mimicOrigin = {0.0f, 0.0f};

                        Color mimicTint = (mimicHitFlashTimer[i] > 0.0f) ? RED : WHITE;

                        // DrawRectangle(mimics[i].x, mimics[i].y, 100, 200, RED); actual mimichitbox
                        DrawTexturePro(mimicTex, sourceRec, destRec, mimicOrigin, 0.0f, mimicTint);

                        // Attack-impact particle burst -- plays the instant the swing starts,
                        // centered on the snapshotted attackrect rather than stretched into
                        // it (attackrect's aspect is a hitbox shape, not the particle art's
                        // square canvas). Frame-swap follows the same progress-based pattern
                        // as the double-jump burst, just with 2 frames instead of 3.
                        if (mimicParticleTimer[i] > 0.0f)
                        {
                            float mimicParticleElapsed = MIMIC_PARTICLE_DURATION - mimicParticleTimer[i];
                            float mimicParticleProgress = mimicParticleElapsed / MIMIC_PARTICLE_DURATION;
                            int mimicParticleFrame = (int)(mimicParticleProgress * 2.0f);
                            if (mimicParticleFrame > 1)
                                mimicParticleFrame = 1;
                            if (mimicParticleFrame < 0)
                                mimicParticleFrame = 0;

                            Texture2D mimicParticleTex = texMimicParticle[mimicParticleFrame];
                            Rectangle mimicParticleSource = {0.0f, 0.0f, (float)mimicParticleTex.width, (float)mimicParticleTex.height};

                            float mimicParticleDrawSize = MIMIC_PARTICLE_SIZE;
                            Vector2 mimicParticleCenter = {
                                mimicParticleRect[i].x + mimicParticleRect[i].width / 2.0f,
                                mimicParticleRect[i].y + mimicParticleRect[i].height / 2.0f};

                            Rectangle mimicParticleDest = {
                                mimicParticleCenter.x - mimicParticleDrawSize / 2.0f,
                                mimicParticleCenter.y - mimicParticleDrawSize / 2.0f,
                                mimicParticleDrawSize,
                                mimicParticleDrawSize};

                            DrawTexturePro(mimicParticleTex, mimicParticleSource, mimicParticleDest, (Vector2){0.0f, 0.0f}, 0.0f, WHITE);
                        }
                    }
                }
                // --- Draw Archer Enemies (Hitbox + Sprite) ---
                for (int i = 0; i < archerCount; i++)
                {
                    if (archers[i].alive && archers[i].level == currentLevel)
                    {
                        // 1. Draw the Hitbox Reference
                        // DrawRectangle(archers[i].x, archers[i].y, 100, 200, (archerHitFlashTimer[i] > 0.0f) ? RED : PURPLE);

                        // 2. Determine and Draw the Sprite
                        Texture2D currentArcherTex = texArcherIdle;

                        // Detect if the archer just fired (timer jumps back up to max)
                        if (archers[i].attacktimer > archerPrevAttackTimer[i] + 0.5f)
                        {
                            archerSpawnTimer[i] = 0.2f; // Trigger 2-frame spawn animation
                        }
                        archerPrevAttackTimer[i] = archers[i].attacktimer;

                        // Detect movement to trigger walk cycle
                        bool isWalking = (fabs(archers[i].x - archerPrevX[i]) > 0.5f);
                        archerPrevX[i] = archers[i].x;

                        // State Machine: Spawn -> Attack -> Walk -> Idle
                        if (archerSpawnTimer[i] > 0.0f)
                        {
                            archerSpawnTimer[i] -= dt;
                            int frame = (archerSpawnTimer[i] > 0.1f) ? 0 : 1;
                            currentArcherTex = texArcherSpawn[frame];
                        }
                        else if (archers[i].attacktimer < 0.4f && archers[i].attacktimer > 0.0f)
                        {
                            // Play 4-frame throw animation in the final 0.4s before firing
                            int frame = (int)((0.4f - archers[i].attacktimer) / 0.1f);
                            if (frame > 3)
                                frame = 3;
                            if (frame < 0)
                                frame = 0;
                            currentArcherTex = texArcherAttack[frame];
                        }
                        else if (isWalking)
                        {
                            currentArcherTex = texArcherWalk[currentArcherWalkFrame];
                        }
                        else
                        {
                            currentArcherTex = texArcherIdle;
                        }

                        // Handle Direction / Horizontal Flip
                        float sourceWidth = (float)currentArcherTex.width;
                        if (archers[i].direction == -1)
                        {
                            sourceWidth = -sourceWidth;
                        }
                        Rectangle sourceRec = {0.0f, 0.0f, sourceWidth, (float)currentArcherTex.height};

                        // Maintain Aspect Ratio based on the 200px tall hitbox, scaled up by 30%
                        float archerAspect = (float)currentArcherTex.width / (float)currentArcherTex.height;
                        float archerDrawHeight = 260.0f; // Increased by 30% (from 200.0f)
                        float archerDrawWidth = archerAspect * archerDrawHeight;

                        // Center horizontally over the 100px hitbox
                        float offsetX = (archerDrawWidth - 100.0f) / 2.0f;
                        // Offset vertically so the larger sprite doesn't sink into the floor
                        float offsetY = archerDrawHeight - 200.0f;

                        Rectangle destRec = {
                            archers[i].x - offsetX,
                            archers[i].y - offsetY,
                            archerDrawWidth,
                            archerDrawHeight};

                        Color archerTint = (archerHitFlashTimer[i] > 0.0f) ? RED : WHITE;
                        DrawTexturePro(currentArcherTex, sourceRec, destRec, (Vector2){0, 0}, 0.0f, archerTint);
                    }
                }
                for (int i = 0; i < totemCount; i++)
                {
                    if (totems[i].alive && totems[i].level == currentLevel)
                    {
                        // DrawRectangle(totems[i].x, totems[i].y, 100, 150, (totemHitFlashTimer[i] > 0.0f) ? RED : DARKPURPLE); // Old placeholder

                        Texture2D currentTotemTex = texTotem[currentTotemFrame];
                        Rectangle sourceRec = {0.0f, 0.0f, (float)currentTotemTex.width, (float)currentTotemTex.height};

                        // Scale based on the 150px tall hitbox height to maintain native aspect ratio
                        float totemAspect = (float)currentTotemTex.width / (float)currentTotemTex.height;
                        float totemDrawHeight = 350.0f; // Set this higher if your sprite should be larger than the hitbox
                        float totemDrawWidth = totemAspect * totemDrawHeight;

                        // Center horizontally over the 100px width hitbox and align feet to the bottom
                        float offsetX = (totemDrawWidth - 100.0f) / 2.0f;
                        float offsetY = totemDrawHeight - 150.0f;

                        Rectangle destRec = {
                            totems[i].x - offsetX,
                            totems[i].y - offsetY,
                            totemDrawWidth,
                            totemDrawHeight};

                        // Retain the red damage flash logic
                        Color totemTint = (totemHitFlashTimer[i] > 0.0f) ? RED : WHITE;

                        DrawTexturePro(currentTotemTex, sourceRec, destRec, (Vector2){0, 0}, 0.0f, totemTint);
                    }
                }
                for (int i = 0; i < MAX_HOMING_BULLETS; i++)
                {
                    if (homingBullets[i].alive)
                        DrawCircle(homingBullets[i].x, homingBullets[i].y, 15, PINK);
                }
                // if (dragon.alive)
                //     DrawRectangle(dragon.x, dragon.y, 300, 200, DARKGREEN);
                // UFO Drawing Start
                if (dragon.alive && dragon.level == currentLevel)
                {
                    int ufoFrame;
                    if (dragon.dstate == Dattacking)
                    {
                        // last 0.3s of the beam window plays the "winding down" frame
                        ufoFrame = (dragon.attacktimer > 0.3f) ? 0 : 1;
                    }
                    else
                    {
                        ufoFrame = 2; // beam off / idle
                    }

                    float frameWidth = (float)texUFO.width / 3.0f;
                    float srcW = frameWidth;
                    if (dragon.direction == -1)
                        srcW = -srcW; // same flip convention as spiritChase/currentTex

                    Rectangle ufoSrc = {ufoFrame * frameWidth, 0.0f, srcW, (float)texUFO.height};
                    Rectangle ufoDest = {dragon.x, dragon.y, 375.0f, 250.0f};
                    Vector2 ufoOrigin = {0.0f, 0.0f};

                    Color dragonTint = (dragonHitFlashTimer > 0.0f) ? RED : WHITE;

                    DrawTexturePro(texUFO, ufoSrc, ufoDest, ufoOrigin, 0.0f, dragonTint);
                }
                // --- Draw UFO Laser Beam ---
                // dragon.firerect (built in enemies.c) is a tall vertical rectangle
                // firing straight DOWN from the UFO's belly (width 300, height 1500),
                // roughly centered under the sprite -- not a sideways bolt, so there's
                // no direction-based flip/offset here.
                if (dragon.dstate == Dattacking && dragon.alive == true)
                {
                    Texture2D currentBeamTex = texBeam[currentBeamFrame];
                    Rectangle beamSrc = {0.0f, 0.0f, (float)currentBeamTex.width, (float)currentBeamTex.height};
                    Color beamTint = (dragonHitFlashTimer > 0.0f) ? RED : WHITE;

                    // Visual-only fix for the gap between the UFO's belly and the beam:
                    // firerect.y already starts exactly at the UFO sprite's bottom edge,
                    // so the visible gap is almost certainly empty/transparent padding
                    // baked into the top of the beam1-5.png art itself. Pull the drawn
                    // rectangle's top edge up (behind/into the ship sprite, so the padding
                    // is hidden there) and grow the height to compensate. The real hitbox
                    // (dragon.firerect, used for collision above) is left untouched.
                    // Raise this if a gap is still visible; lower/zero it if the beam art
                    // now pokes up too far into the ship.
                    const float BEAM_VISUAL_OVERLAP = 30.0f;
                    Rectangle beamDest = dragon.firerect;
                    beamDest.y -= BEAM_VISUAL_OVERLAP;
                    beamDest.height += BEAM_VISUAL_OVERLAP;

                    DrawTexturePro(currentBeamTex, beamSrc, beamDest, (Vector2){0.0f, 0.0f}, 0.0f, beamTint);
                }

                // --- Draw Cake Arrows ---
                for (int i = 0; i < MAX_ARROWS; i++)
                {
                    if (arrows[i].alive)
                    {
                        float cakeDrawSize = 85.0f;
                        Rectangle cakeSrc = {0.0f, 0.0f, (float)texCake.width, (float)texCake.height};

                        // Center cake visually on the coordinate point
                        Rectangle cakeDest = {
                            arrows[i].x - cakeDrawSize / 2.0f,
                            arrows[i].y - cakeDrawSize / 2.0f,
                            cakeDrawSize,
                            cakeDrawSize};
                        DrawTexturePro(texCake, cakeSrc, cakeDest, (Vector2){0, 0}, 0.0f, WHITE);
                    }
                }
                EndMode2D();
                DrawText(TextFormat("Dash Cooldown: %.1f", P.dashcooldown), 20, 40, 30, WHITE);
                DrawRectangle(20, 20, 200, 20, DARKGRAY);                       // health bar background grey
                DrawRectangle(20, 20, 200 * (P.health / P.maxHealth), 20, RED); // foreground — width = maxWidth * (health / maxHealth)
                EndDrawing();
            }
        }

        // --- FIX: Gameover and Win are now handled as top-level states in the
        // main loop, siblings of `if (state == Playing)`, instead of being
        // nested inside it. Previously both were only reachable on the exact
        // frame the state transition happened (because `if (state == Playing)`
        // is only evaluated once per frame, at loop-top): the frame gate
        // changed `state` to Win/Gameover mid-block, that block finished with
        // one extra draw, and then on every subsequent frame the outer
        // `if (state == Playing)` was false, so the nested Gameover/Win blocks
        // -- including their own BeginDrawing/EndDrawing and KEY_ENTER checks
        // -- became completely unreachable. That left the framebuffer never
        // swapping again (a frozen screen) and ENTER doing nothing. Moving
        // them out here means they get polled and drawn every single frame
        // for as long as `state` holds that value.
        if (state == Gameover)
        {
            if (IsKeyPressed(KEY_ENTER))
            {
                state = Mainmenu; // no type, just assignment
                currentLevel = 0; // reset to the same level the game boots into
                Vector2 resetSpawn = GetLevelBottomLeftSpawn(currentLevel);
                P.x = resetSpawn.x;
                P.y = resetSpawn.y;
                P.health = 1000.0f;
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

                // Level 2 upper-platform pool -- back to dormant, fresh counts;
                // the spawner in the main loop brings the first one in.
                en2.alive = false;
                en2.spiritcollision = false;
                en2.knockbackduration = 0;
                spiritsToSpawn = 3;
                dragonsToSpawn = 3;

                for (int i = 0; i < mimicCount; i++)
                {
                    mimics[i].alive = true;
                    mimics[i].health = 100.0f;
                    mimics[i].mstate = MIdle;
                    mimics[i].playerknockbacktimer = 0;
                    mimics[i].knockbackduration = 0;
                    mimicPrevHealth[i] = 100.0f;
                    mimicHitFlashTimer[i] = 0.0f;
                    mimicAttackAnimActive[i] = false;
                    mimicAttackAnimTimer[i] = 0.0f;
                    mimicParticleTimer[i] = 0.0f;
                    mimicParticleRect[i] = (Rectangle){0};
                }

                for (int i = 0; i < bullCount; i++)
                {
                    bulls[i].alive = true;
                    bulls[i].health = 90.0f;
                    bulls[i].state = Idle;
                    bulls[i].speed = 100.0f;
                    bullPrevHealth[i] = 90.0f;
                    bullHitFlashTimer[i] = 0.0f;
                }
                for (int i = 0; i < archerCount; i++)
                {
                    archers[i].alive = true;
                    archers[i].health = 80.0f;
                    archers[i].Astate = AIdle;
                    archers[i].attacktimer = 2.0f;
                    archerPrevHealth[i] = 80.0f;
                    archerHitFlashTimer[i] = 0.0f;
                }
                for (int i = 0; i < MAX_ARROWS; i++)
                    arrows[i].alive = false;

                dragon.alive = false;
                dragon.health = 50.0f;
                dragon.dstate = Didle;
                dragon.x = 1500.0f;
                dragon.y = 500.0f;
                dragon.wallDropSpeed = 0;
                dragon.playerknockbacktimer = 0;
                dragon.playerecoil = 0;
                dragonPrevHealth = 50.0f;
                dragonHitFlashTimer = 0.0f;

                for (int i = 0; i < totemCount; i++)
                {
                    totems[i].alive = true;
                    totems[i].health = 60.0f;
                    totems[i].attacktimer = totems[i].maxattacktimer;
                    totems[i].knockbackduration = 0;
                    totemPrevHealth[i] = 60.0f;
                    totemHitFlashTimer[i] = 0.0f;
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

        if (state == Win)
        {
            BeginDrawing();
            ClearBackground(BLACK);
            Rectangle winSrc = {0, 0, (float)texWin.width, (float)texWin.height};
            Rectangle winDest = {0, 0, (float)screen_w, (float)screen_h};
            DrawTexturePro(texWin, winSrc, winDest, (Vector2){0, 0}, 0.0f, WHITE);
            EndDrawing();

            if (IsKeyPressed(KEY_ENTER))
            {
                // Run complete -- close the game instead of looping back
                // to the main menu.
                //
                // Must drop out of exclusive fullscreen and restore the
                // cursor before CloseWindow(), otherwise some
                // drivers/OSes fail to release the display and the
                // whole screen locks up instead of just closing.
                if (IsWindowFullscreen())
                    ToggleFullscreen();
                ShowCursor();
                goto shutdown;
            }
        }
        // --- NEW: Unload textures before closing ---
    }
shutdown:
// --- Unload Level Background Textures ---
// --- Unload Level Background & Tile Textures ---
for (int i = 0; i < 3; i++)
{
    UnloadTexture(texLevelBG[i]);
    UnloadTexture(texTile[i]); // NEW: Unload tile textures
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

    UnloadTexture(texUFO);
    // --- Unload UFO Laser Beam Textures ---
    for (int i = 0; i < 5; i++)
        UnloadTexture(texBeam[i]);

    // --- Unload Bull Textures ---
    UnloadTexture(texBullIdle);
    for (int i = 0; i < 4; i++)
        UnloadTexture(texBullRun[i]);
    for (int i = 0; i < 2; i++)
        UnloadTexture(texBullStop[i]);

    // --- Unload Mimic Textures ---
    UnloadTexture(texMimicIdle);
    for (int i = 0; i < 2; i++)
        UnloadTexture(texMimicRun[i]);
    for (int i = 0; i < 2; i++)
        UnloadTexture(texMimicAttack[i]);
    UnloadTexture(texMimicCharge);
    for (int i = 0; i < 2; i++)
        UnloadTexture(texMimicParticle[i]);
    UnloadTexture(texArcherIdle);
    for (int i = 0; i < 2; i++)
        UnloadTexture(texArcherWalk[i]);
    for (int i = 0; i < 4; i++)
        UnloadTexture(texArcherAttack[i]);
    for (int i = 0; i < 2; i++)
        UnloadTexture(texArcherSpawn[i]);
    UnloadTexture(texCake);
    // --- Unload Totem Textures ---
    for (int i = 0; i < 4; i++)
    {
        UnloadTexture(texTotem[i]);
    }

    // --- Unload Spike Textures ---
    for (int i = 0; i < 2; i++)
    {
        UnloadTexture(texSpike[i]);
    }
    // --- Unload Pause Menu Texture ---
    UnloadTexture(texPauseMenu);

    // --- Unload Win Screen Texture ---
    UnloadTexture(texWin);

    CloseWindow();
    return 0;
}