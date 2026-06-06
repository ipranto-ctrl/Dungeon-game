#include "enemies.h"
#include "player.h"
#include "tilemap.h"
#include <raylib.h>
#include <math.h>
#include <stdlib.h>

void MimicCollisionX(Mimic *M)
{
    int tileX = (int)(M->x / TILE_SIZE);
    int tileY = (int)(M->y / TILE_SIZE);

    for (int i = tileY - 1; i <= tileY + 2; i++)
    {
        for (int j = tileX - 1; j <= tileX + 2; j++)
        {
            if (i < 0 || i >= MAP_ROWS || j < 0 || j >= MAP_COLS)
                continue;
            if (maps[currentLevel][i][j] == 1)
            {
                Rectangle tileRect = {j * TILE_SIZE, i * TILE_SIZE, TILE_SIZE, TILE_SIZE};
                Rectangle mimicrect = {M->x, M->y, 100, 200};
                if (CheckCollisionRecs(mimicrect, tileRect))
                {
                    float overlapLeft = (M->x + 100) - tileRect.x;
                    float overlapRight = (tileRect.x + tileRect.width) - M->x;
                    if (overlapLeft < overlapRight)
                        M->x = tileRect.x - 100;
                    else
                        M->x = tileRect.x + tileRect.width;
                }
            }
        }
    }
}

void MimicCollisionY(Mimic *M)
{
    int tileX = (int)(M->x / TILE_SIZE);
    int tileY = (int)(M->y / TILE_SIZE);

    for (int i = tileY - 1; i <= tileY + 2; i++)
    {
        for (int j = tileX - 1; j <= tileX + 2; j++)
        {
            if (i < 0 || i >= MAP_ROWS || j < 0 || j >= MAP_COLS)
                continue;
            if (maps[currentLevel][i][j] == 1)
            {
                Rectangle tileRect = {j * TILE_SIZE, i * TILE_SIZE, TILE_SIZE, TILE_SIZE};
                Rectangle mimicrect = {M->x, M->y, 100, 200};
                if (CheckCollisionRecs(tileRect, mimicrect))
                {
                    float overlaptop = (M->y + 200) - tileRect.y;
                    float overlapbottom = (tileRect.y + tileRect.height) - M->y;
                    if (overlaptop < overlapbottom)
                    {
                        M->y = tileRect.y - 200;
                        M->velocityY = 0;
                        M->onground = true; // add this
                    }
                    else
                    {
                        M->y = tileRect.y + tileRect.height;
                        M->velocityY = 0;
                    }
                }
            }
        }
    }
}

void UpdateMimicGravity(Mimic *M, float dt)
{
    M->velocityY += M->gravity * dt;
    M->y += M->velocityY * dt;
    M->onground = false; // reset every frame
}

int UpdateMimicLogic(Mimic *M, Player *P, float dt, int attackcheck, Rectangle *AttackRect)
{
    if (M->health <= 0)
        M->alive = false;
    if (M->alive == false)
        return 0;
    Rectangle mimicrect = {M->x, M->y, 100, 200};
    Rectangle playerrect = {P->x, P->y, 100, 200};
    if (P->dashing == false)
    {
        if (CheckCollisionRecs(mimicrect, playerrect))
        {
            if (P->iframes <= 0)
            {
                P->health -= M->damage;
                P->iframes = invincibility;
            }
            M->mstate = MIdle;
            M->speed = 0;

            float overlapright = fabsf(P->x - (M->x + 100));
            float overlapleft = fabsf(M->x - (P->x + 100));
            float overlaptop = fabsf((P->y + 200) - M->y);
            float overlapbottom = fabsf((M->y + 200) - P->y);

            float minOverlap = fminf(fminf(overlapleft, overlapright), fminf(overlaptop, overlapbottom));

            if (minOverlap == overlapleft)
            {
                M->x = P->x + 100;
                P->x -= 50;
            }
            if (minOverlap == overlapright)
            {
                M->x = P->x - 200;
                P->x += 50;
            }
            if (minOverlap == overlaptop)
                P->velocityY = -800.0f;
            if (minOverlap == overlapbottom)
                P->velocityY = 800.0f;
        }
    }
    if (fabs(P->x - M->x) <= 200)
    {
        M->mstate = MCharging;
    }
    else if (fabs(P->x - M->x) < 10 * TILE_SIZE)
    {
        M->mstate = MChasing;
    }
    else if (fabs(P->x - M->x) > 10 * TILE_SIZE)
    {
        M->mstate = MIdle;
    }
    if (M->mstate == MChasing)
    {
        M->speed = M->maxspeed;
        if (P->x > M->x)
        {
            M->direction = 1;
        }
        else
            M->direction = -1;
        M->x += M->direction * M->speed * dt;
    }

    int attackcheckmimic = 0;
    if (M->mstate == MCharging)
    {
        M->attackcooldown -= dt;
        if (M->attackcooldown <= 0)
        {
            attackcheckmimic = 1;
            M->attackcooldown = 1.0;
            if (M->direction == 1)
            {
                M->attackrect.x = M->x + 100;
                M->attackrect.y = M->y;
                M->attackrect.height = 200;
                M->attackrect.width = 200;
            }
            if (M->direction == -1)
            {
                M->attackrect.x = M->x - 200;
                M->attackrect.y = M->y;
                M->attackrect.height = 200;
                M->attackrect.width = 200;
            }
        }
    }

    if (attackcheckmimic)
    {
        if (CheckCollisionRecs(M->attackrect, playerrect))
        {
            if (P->iframes <= 0)
            {
                P->health -= M->damage;
                P->iframes = invincibility;
            }
            M->playerknockbacktimer = 0.3f; // set here only
        }
    }
    if (M->playerknockbacktimer > 0)
    {
        P->x += M->direction * dt * 3000;
        M->x -= M->direction * dt * 1500;
        M->playerknockbacktimer -= dt;
    }
    if (attackcheck)
    {
        Rectangle mimicrect = {M->x, M->y, 100, 200};
        if (CheckCollisionRecs(mimicrect, *AttackRect))
        {
            M->health -= P->damage;
            M->knockbackduration = .01f;
        }
        if (M->knockbackduration > 0)
        {
            P->x -= 3000 * P->dashflag * dt;
            M->x += 3000 * P->dashflag * dt;
        }
    }
    return attackcheckmimic;
}