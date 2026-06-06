#ifndef PLAYER_H
#define PLAYER_H

#include <raylib.h>

extern float invincibility;
typedef struct
{
    float x;
    float speed;
    float dashtimer;
    int dashflag;
    float dashcooldown;

    float y;
    float gravity;
    float velocityY;

    float damage;
    float attackcooldown;

    float health;
    float maxHealth;
    float iframes;

    bool onground;
    bool doublejump;
    bool dashing;
    bool alive;

    float spikeknkbacktimer;
    int spikeknkdirection;
} Player;

void UpdateDash(Player *P, float dt);
void UpdateMovementX(Player *P, float dt);
void UpdateJump(Player *P, float dt);
void CollisionX(Player *P,float dt);
void UpdateGravity(Player *P, float dt);
void CollisionY(Player *P);
int UpdateAttack(Player *P, float dt, Rectangle *AttackRect);
#endif