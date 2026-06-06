#ifndef ENEMIES_H
#define ENEMIES_H

#include <raylib.h>
#include "player.h"

typedef enum {
    Idle,
    Charging,
    Stopping,
    Turning,
} Bullstate;

typedef enum {
    MIdle,
    MCharging,
    MChasing
} Mimicstate;
typedef struct
{
    float x;
    float y;
    float speed;
    float accelaration;
    float deccelaration;
    float health;
    float damage;
    int   direction;
    float gravity;
    float velocityY;
    float turntimer;
    float hittimer;
    float knockbacktimer;
    int   knockbackdirection;
    int   collisiondirection;
    Bullstate state;
    bool  alive;
} Bull;

typedef struct
{
    float x;
    float y;
    float speed;
    float damage;
    float cooldown;
    float knockbackduration;
    bool  alive;
    bool  spiritcollision;
    int   knockdir;
    int   knockdirY;
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
} Mimic;

void BullCollisionX(Bull *B);
void BullCollisionY(Bull *B);
void UpdateBullGravity(Bull *B, float dt);
void spiritupdate(Spirit *en, Player *P, float dt);
void BullUpdateLogic(Bull *bn, Player *P, float dt, int AttackCheck, Rectangle *AttackRect);
void MimicCollisionX(Mimic *M);
void MimicCollisionY(Mimic *M);
void UpdateMimicGravity(Mimic *M, float dt);
int UpdateMimicLogic(Mimic *M, Player *P, float dt, int attackcheck, Rectangle *AttackRect);
#endif

