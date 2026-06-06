#include "enemies.h"
#include "player.h"
#include "tilemap.h"
#include <raylib.h>
#include <math.h>
#include <stdlib.h>

float invincibility=.2f;

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
            en->cooldown = 0.5f;
        }
    }

    // phase 2: waiting to explode
    if (en->spiritcollision == true && en->knockbackduration <= 0)
    {
        en->cooldown -= dt;
        if (en->cooldown <= 0)
        {
            en->knockbackduration = 0.3f;
            if (fabsf(P->x - en->x) < 500 && fabsf(P->y - en->y) < 500)
            {
                if (P->iframes <= 0)
                {
                    P->health -= en->damage;
                    P->iframes = invincibility;
                }
                en->knockdir = (P->x > en->x) ? 1 : -1;
                en->knockdirY = (P->y + 100 < en->y + 25) ? -1 : 1;
            }
            else
                en->knockdir = 0;
        }
    }

    // phase 3: knockback
    if (en->knockbackduration > 0)
    {
        if (en->knockdir != 0)
        {
            P->x += en->knockdir * 3000.0f * dt;
            P->velocityY = en->knockdirY * 2000.0f;
        }
        en->knockbackduration -= dt;
        if (en->knockbackduration <= 0)
            en->alive = false;
    }
}

void BullCollisionX(Bull *B)
{
    if (B->alive == false)
        return ;
    int tileX = (int)(B->x / TILE_SIZE);
    int tileY = (int)(B->y / TILE_SIZE);

    for (int i = tileY - 1; i <= tileY + 2; i++)
    {
        for (int j = tileX - 1; j <= tileX + 2; j++)
        {
            if (i < 0 || i >= MAP_ROWS || j < 0 || j >= MAP_COLS)
                continue;
            if (maps[currentLevel][i][j] == 1)
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
    if (B->alive == false)
        return ;
    int tileX = (int)(B->x / TILE_SIZE);
    int tileY = (int)(B->y / TILE_SIZE);

    for (int i = tileY - 1; i <= tileY + 2; i++)
    {
        for (int j = tileX - 1; j <= tileX + 2; j++)
        {
            if (i < 0 || i >= MAP_ROWS || j < 0 || j >= MAP_COLS)
                continue;
            if (maps[currentLevel][i][j] == 1)
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

void UpdateBullGravity(Bull *B, float dt)
{
    if (B->alive == false)
        return ;
    B->velocityY += B->gravity * dt;
    B->y += B->velocityY * dt;
}

void BullUpdateLogic(Bull *bn, Player *P, float dt, int AttackCheck, Rectangle *AttackRect)
{
    if (bn->health <= 0)
    {
        bn->alive = false;
    }
    if (bn->alive == false)
    {
        return;
    }

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
        bn->turntimer -= dt;
        if (bn->turntimer <= 0)
        {
            bn->turntimer = 1.0f;
            bn->state = Idle;
        }
    }

    Rectangle bullrect = {bn->x, bn->y, 200, 200};
    Rectangle playerrect = {P->x, P->y, 100, 200};

    if (P->dashing == false)
    {
        if (CheckCollisionRecs(bullrect, playerrect))
        {
            if (P->iframes <= 0)
            {
                P->health -= bn->damage;
                P->iframes = invincibility;
            }
            bn->state = Turning;
            bn->speed = 0;
            bn->hittimer = 0.3f;

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

            bn->collisiondirection = (overlapleft < overlapright) ? -1 : 1;
        }
        if (bn->hittimer > 0)
        {
            bn->hittimer -= dt;
            float playerspeed = 1500.0f;
            P->x += bn->collisiondirection * playerspeed * dt;
            playerspeed -= 200 * dt;
        }
        if (bn->hittimer <= 0)
            bn->hittimer = 0;
    }

    if (AttackCheck)
    {
        if (CheckCollisionRecs(bullrect, *AttackRect))
        {
            bn->health -= P->damage;
            float atkoverlapright = fabsf(AttackRect->x - (bn->x + 200));
            float atkoverlapleft = fabsf((AttackRect->x + AttackRect->width) - bn->x);
            float minatkoverlap = fminf(atkoverlapleft, atkoverlapright);
            bn->knockbacktimer = 0.01f;
            if (minatkoverlap == atkoverlapleft)
                bn->knockbackdirection = 1;
            if (minatkoverlap == atkoverlapright)
                bn->knockbackdirection = -1;
        }
    }
    if (bn->knockbacktimer > 0)
    {
        bn->x += bn->knockbackdirection * dt * 3500;
        P->x -= bn->knockbackdirection * dt * 5000;
    }
    bn->knockbacktimer -= dt;
    if (bn->knockbacktimer < 0)
        bn->knockbacktimer = 0;
}