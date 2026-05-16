#ifndef ENEMIES_H
#define ENEMIES_H

#include<raylib.h>
#include"player.h"

extern float bullhitimer;
extern bool hitstate;
extern float knockbacktimer;//for bull
extern int knockbackdirection;//for bull

typedef enum {
    Idle,
    Charging,
    Stopping,
    Turning,
}Bullstate;

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
    Bullstate state;
    bool alive;
    
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
}Spirit;

void BullCollisionX(Bull *B);

void BullCollisionY(Bull *B);

void UpdateBullGravity(Bull *B, float dt);

void spiritupdate(Spirit *en,Player *P,float dt);

void BullUpdateLogic(Bull *bn, Player *P, float dt, float *timer, float *bullhitimer, bool *hitstate,int AttackCheck,Rectangle *AttackRect);

#endif
