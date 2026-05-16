#include "enemies.h"
#include "player.h"
#include "tilemap.h"
#include <raylib.h>
#include <math.h>
#include <stdlib.h>

int CollisionDirectionBull = 1;
float bullhitimer = 0;
float knockbacktimer = 0;   // for bull
int knockbackdirection = 1; // for bull

void spiritupdate(Spirit *en, Player *P, float dt)
{
    if (en->alive == false)
        return;

    // phase 1: chasing
    if (en->spiritcollision == false)
    {
        if (IsKeyDown(KEY_A) || IsKeyDown(KEY_D))
            en->speed += 600.0f * dt;
        else
            en->speed = 600.0f;

        float dx = P->x - en->x;
        float dy = P->y - en->y;
        float dist = sqrtf(dx * dx + dy * dy);

        if (dist > 1.0f)
        {
            en->x += (dx / dist) * en->speed * dt;
            en->y += (dy / dist) * en->speed * dt;
        }

        Rectangle spiritrec = {en->x, en->y, 50, 50};
        Rectangle playerrect = {P->x, P->y, 100, 200};
        if (CheckCollisionRecs(spiritrec, playerrect))
        {
            en->spiritcollision = true;
            en->cooldown = 0.5f; // wait .5 second before exploding
        }
    }

    // phase 2: waiting to explode
    if (en->spiritcollision == true && en->knockbackduration <= 0)
    {
        en->cooldown -= dt;
        if (en->cooldown <= 0)
        {
            en->knockbackduration = 0.3f;
            if (fabsf(P->x - en->x) < 500) // 50 is spirit size
            {    en->knockdir = (P->x > en->x) ? 1 : -1;
                en->knockdirY =(P->y+100 < en->y+25) ? -1:1;
            }
            else
                en->knockdir = 0; // no knockback
        }
    }

    // phase 3: knockback
    if (en->knockbackduration > 0)
    {

        if (en->knockdir != 0)
        {
            P->x += en->knockdir * 3000.0f * dt;
            P->velocityY = en->knockdirY*2000;
        }
        en->knockbackduration -= dt;
        if (en->knockbackduration <= 0)
            en->alive = false;
    }
}

void BullCollisionX(Bull *B)
{
    for (int i = 0; i < MAP_ROWS; i++)
    {
        for (int j = 0; j < MAP_COLS; j++)
        {
            if (map[i][j] == 1)
            {
                Rectangle tileRect = {j * TILE_SIZE, i * TILE_SIZE, TILE_SIZE, TILE_SIZE};
                Rectangle bullRect = {B->x, B->y, 200, 200};
                if (CheckCollisionRecs(bullRect, tileRect))
                {
                    float overlapLeft = (B->x + 200) - tileRect.x;
                    float overlapRight = (tileRect.x + tileRect.width) - B->x;
                    if (overlapLeft < overlapRight)
                    {
                        B->x = tileRect.x - 200;
                        B->state = Turning;
                        B->speed = 0;
                    }
                    else
                    {
                        B->x = tileRect.x + tileRect.width;
                        B->state = Turning;
                        B->speed = 0;
                    }
                }
            }
        }
    }
}

void BullCollisionY(Bull *B)
{
    for (int i = 0; i < MAP_ROWS; i++)
    {
        for (int j = 0; j < MAP_COLS; j++)
        {
            if (map[i][j] == 1)
            {
                Rectangle tileRect = {j * TILE_SIZE, i * TILE_SIZE, TILE_SIZE, TILE_SIZE};
                Rectangle bullRect = {B->x, B->y, 200, 200};
                if (CheckCollisionRecs(bullRect, tileRect))
                {
                    float overlapTop = (B->y + 200) - tileRect.y;
                    float overlapBottom = (tileRect.y + tileRect.height) - B->y;
                    if (overlapTop < overlapBottom)
                    {
                        B->y = tileRect.y - 200;
                        B->velocityY = 0;
                    }
                    else
                    {
                        B->y = tileRect.y + tileRect.height;
                        B->velocityY = 0;
                    }
                }
            }
        }
    }
}

void BullUpdateLogic(Bull *bn, Player *P, float dt, float *timer, float *bullhitimer, bool *hitstate, int AttackCheck, Rectangle *AttackRect)
{
    if (bn->state == Idle)
    {
        if (fabs(P->y - bn->y) <= 250 && fabs(P->x - bn->x) <= 15 * TILE_SIZE)
        {
            bn->speed = 800.0f;
            bn->state = Charging;
        }
        if (P->x > bn->x)
            bn->direction = 1;
        else
            bn->direction = -1;
    }
    if (bn->state == Charging)
    {
        bn->speed += bn->accelaration * dt;
        bn->x += bn->direction * bn->speed * dt;
        if (bn->direction * (P->x - bn->x) < 0)
            bn->state = Stopping;
    }
    if (bn->state == Stopping)
    {
        bn->speed -= bn->deccelaration * dt;
        bn->x += bn->direction * bn->speed * dt;
        if (bn->speed <= 0)
            bn->state = Turning;
    }
    if (bn->state == Turning)
    {
        if (*timer <= 0)
        {
            *timer = 1;
            bn->state = Idle;
        }
        *timer -= dt;
    }

    Rectangle bullrect = {bn->x, bn->y, 200, 200};
    Rectangle playerrect = {P->x, P->y, 100, 200};
    if (P->dashing == false)
    {
        if (CheckCollisionRecs(bullrect, playerrect))
        {
            bn->state = Turning;
            bn->speed = 0;
            *bullhitimer = 0.3f;

            float overlapright = fabsf(P->x - (bn->x + 200));
            float overlapleft = fabsf(bn->x - (P->x + 100));
            float overlaptop = fabsf((P->y + 200) - bn->y);
            float overlapbottom = fabsf((bn->y + 200) - P->y);

            float minOverlap = fminf(fminf(overlapleft, overlapright), fminf(overlaptop, overlapbottom));

            if (minOverlap == overlapleft)
            {
                bn->x = P->x + 100;
                P->x -= 50;
            }
            if (minOverlap == overlapright)
            {
                bn->x = P->x - 200;
                P->x += 50;
            }
            if (minOverlap == overlaptop)
                P->velocityY = -800.0f;
            if (minOverlap == overlapbottom)
                P->velocityY = 800.0f;
            if (overlapleft < overlapright)
                CollisionDirectionBull = -1;
            else
                CollisionDirectionBull = 1;
        }
        if (*bullhitimer > 0)
        {
            *bullhitimer -= dt;
            float playerspeed = 1500.0f;
            P->x += CollisionDirectionBull * playerspeed * dt;
            playerspeed -= 200 * dt;
        }
        if (*bullhitimer <= 0)
        {
            *hitstate = false;
            *bullhitimer = 0;
        }
    }

    if (AttackCheck)
    {
        if (CheckCollisionRecs(bullrect, *AttackRect))
        {
            float atkoverlapright = fabsf(AttackRect->x - (bn->x + 200));
            float atkoverlapleft = fabsf((AttackRect->x + AttackRect->width) - bn->x);
            float minatkoverlap = fminf(atkoverlapleft, atkoverlapright);
            knockbacktimer = .01;
            if (minatkoverlap == atkoverlapleft)
            {
                knockbackdirection = +1;
            }
            if (minatkoverlap == atkoverlapright)
            {
                knockbackdirection = -1;
            }
        }
    }
    if (knockbacktimer > 0)
    {
        bn->x += knockbackdirection * dt * 2000;
        P->x -= knockbackdirection * dt * 5000;
    }
    knockbacktimer -= dt;
    if (knockbacktimer < 0)
        knockbacktimer = 0;
}