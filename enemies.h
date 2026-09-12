#ifndef ENEMIES_H
#define ENEMIES_H

#include <raylib.h>
#include "player.h"

#define MAX_ARROWS 30
typedef enum
{
    Idle,
    Charging,
    Stopping,
    Turning,
} Bullstate;

typedef enum
{
    MIdle,
    MCharging,
    MChasing
} Mimicstate;

typedef enum
{
    AIdle,
    AChasing,
    ACharging,
} Archerstate;

typedef enum 
{
    Didle,
    Dchasing,
    Dcharging,
    Dattacking,
}Dragonstate;
typedef struct
{
    float x;
    float y;
    float speed;
    float accelaration;
    float deccelaration;
    float health;
    float damage;
    int direction;
    float gravity;
    float velocityY;
    float turntimer;
    float hittimer;
    float knockbacktimer;
    int knockbackdirection;
    int collisiondirection;
    Bullstate state;
    bool alive;
    float maxturntimer;
    int level; // which currentLevel this enemy spawns/acts on
} Bull;

typedef struct
{
    float x;
    float y;
    float speed;
    float damage;
    float cooldown;
    float knockbackduration;
    bool alive;
    bool spiritcollision;
    int knockdir;
    int knockdirY;
    int level; // which currentLevel this enemy spawns/acts on
} Spirit;

typedef struct
{
    float x;
    float y;
    float velocityY;
    float gravity;
    float speed;
    float health;
    float damage;
    float attacktimer;
    float attackcooldown;
    float jumptimer;
    int direction;
    Mimicstate mstate;
    bool alive;
    Rectangle attackrect;
    bool onground;
    float playerknockbacktimer;
    float knockbackduration;
    float maxspeed;
    int level; // which currentLevel this enemy spawns/acts on
} Mimic;

typedef struct
{
    float x;
    float y;
    float velocityY;
    float gravity;
    float speed;
    float health;
    float damage;
    float attacktimer;
    float jumptimer;
    int direction;
    Archerstate Astate;
    bool alive;
    bool onground;
    float playerknockbacktimer;
    float knockbackduration;
    float maxspeed;
    float arrowdamage;
    float maxattacktimer;
    int level; // which currentLevel this enemy spawns/acts on
} Archer;

typedef struct
{
    float x;
    float y;
    float vx; 
    float vy;
    bool alive;
    float timer;
} Arrow;

typedef struct 
{
    float x;
    float y;
    float health;
    float damage;
    float chargetimer;
    float maxchargetimer;
    float attacktimer;
    float maxattacktimer;
    bool alive;
    int direction;
    Dragonstate dstate;
    Rectangle firerect;
    float knockbackduration;
    float playerknockbacktimer;
    float playerecoil;
    int recoildirection;
    float speed;
    float attackspeed;
    float wallDropSpeed;
    int level; // which currentLevel this enemy spawns/acts on
}Dragon;


#define MAX_HOMING_BULLETS 20

typedef struct
{
    float x;
    float y;
    float health;
    float damage;
    float attacktimer;
    float maxattacktimer;
    bool alive;
    float knockbackduration;
    float playerecoil;
    int recoildirection;
    int level; // which currentLevel this enemy spawns/acts on
} Totem;

typedef struct
{
    float x;
    float y;
    float angle;
    float speed;
    float turnRate;
    float damage;
    bool alive;
} HomingBullet;

void BullCollisionX(Bull *B);
void BullCollisionY(Bull *B);
void UpdateBullGravity(Bull *B, float dt);
void spiritupdate(Spirit *en, Player *P, float dt);
void BullUpdateLogic(Bull *bn, Player *P, float dt, int AttackCheck, Rectangle *AttackRect);
void MimicCollisionX(Mimic *M);
void MimicCollisionY(Mimic *M);
void UpdateMimicGravity(Mimic *M, float dt);
int UpdateMimicLogic(Mimic *M, Player *P, float dt, int attackcheck, Rectangle *AttackRect);
void ArcherCollisionX(Archer *M);
void ArcherCollisionY(Archer *M);
void UpdateArcherGravity(Archer *M, float dt);
int UpdateArcherLogic(Archer *A, Player *P, float dt, int attackcheck, Rectangle *AttackRect, Arrow *arrows, int arrowCount);
void UpdateArrows(Arrow *arrows, int count, Player *P, float dt);
void UpdateDragon(Dragon *D, Player *P, float dt, int attackcheck, Rectangle *attackrect);
void DragonCollisionX(Dragon *D, float dt);
void DragonCollisionY(Dragon *D);
void UpdateTotemLogic(Totem *T, Player *P, float dt, int attackcheck, Rectangle *AttackRect, HomingBullet *bullets, int bulletCount);
void UpdateHomingBullets(HomingBullet *bullets, int count, Player *P, float dt,int attackcheck,Rectangle *Attackrect);
void TotemCollision(Totem *T, Player *P);

#endif