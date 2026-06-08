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
            P->doublejump = true;

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
    else if (fabs(P->x - M->x) < 10 * TILE_SIZE && fabs(P->y - M->y) < 3 * TILE_SIZE)
    {
        M->mstate = MChasing;
    }
    else
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
            M->mstate = MIdle;
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
        if (CheckCollisionRecs(mimicrect, *AttackRect))
        {
            M->health -= P->damage;
            M->knockbackduration = .01f;
        }
        if (M->knockbackduration > 0)
        {
            P->x -= 3000 * P->dashflag * dt;
            M->x += 3000 * P->dashflag * dt;
            M->knockbackduration -= dt;
        }
    }
    return attackcheckmimic;
}

void ArcherCollisionX(Archer *M)
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
                Rectangle Archerrect = {M->x, M->y, 100, 200};
                if (CheckCollisionRecs(Archerrect, tileRect))
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

void ArcherCollisionY(Archer *M)
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
                Rectangle Archerrect = {M->x, M->y, 100, 200};
                if (CheckCollisionRecs(tileRect, Archerrect))
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

void UpdateArcherGravity(Archer *M, float dt)
{
    M->velocityY += M->gravity * dt;
    M->y += M->velocityY * dt;
    M->onground = false; // reset every frame
}

int UpdateArcherLogic(Archer *A, Player *P, float dt, int attackcheck, Rectangle *AttackRect, Arrow *arrows, int arrowCount)
{
    if (A->health <= 0)
        A->alive = false;
    if (A->alive == false)
        return 0;
    Rectangle ArcherRect = {A->x, A->y, 100, 200};
    Rectangle playerrect = {P->x, P->y, 100, 200};
    if (P->dashing == false)
    {
        if (CheckCollisionRecs(ArcherRect, playerrect))
        {
            if (P->iframes <= 0)
            {
                P->health -= A->damage;
                P->iframes = invincibility;
            }
            A->Astate = AIdle;
            A->speed = 0;
            P->doublejump = true;

            float overlapright = fabsf(P->x - (A->x + 100));
            float overlapleft = fabsf(A->x - (P->x + 100));
            float overlaptop = fabsf((P->y + 200) - A->y);
            float overlapbottom = fabsf((A->y + 200) - P->y);

            float minOverlap = fminf(fminf(overlapleft, overlapright), fminf(overlaptop, overlapbottom));

            if (minOverlap == overlapleft)
            {
                A->x = P->x + 100;
                P->x -= 50;
            }
            if (minOverlap == overlapright)
            {
                A->x = P->x - 200;
                P->x += 50;
            }
            if (minOverlap == overlaptop)
                P->velocityY = -800.0f;
            if (minOverlap == overlapbottom)
                P->velocityY = 800.0f;
        }
    }
    if (fabs(P->x - A->x) <= 8 * TILE_SIZE && fabs(P->y - A->y) <= 8 * TILE_SIZE)
    {
        A->Astate = ACharging;
    }
    else if (fabs(P->x - A->x) <= 16 * TILE_SIZE && fabs(P->y - A->y) <= 8 * TILE_SIZE)
    {
        A->Astate = AChasing;
    }
    else
    {
        A->Astate = AIdle;
    }
    if (A->Astate == AIdle)
    {
        A->attacktimer = 1.0f;
    }
    A->speed = A->maxspeed;
    if (P->x > A->x)
    {
        A->direction = 1;
    }
    else
        A->direction = -1;
    if (A->Astate == AChasing)
    {
        A->x += A->direction * A->speed * 1.5 * dt;
    }
    int attackcheckArcher = 0;
    if (A->Astate == ACharging)
    {
        A->attacktimer -= dt;
        if (A->attacktimer <= 0)
        {
            attackcheckArcher = 1;
            A->attacktimer = A->maxattacktimer;
            A->Astate = AIdle;
        }
        if (fabs(P->x - A->x) <= 4 * TILE_SIZE)
            A->x -= A->direction * A->speed * dt;
    }

    if (attackcheckArcher)
    {
        // find a free arrow slot and fire it toward the player
        for (int i = 0; i < arrowCount; i++)
        {
            if (!arrows[i].alive)
            {
                // aim from archer center toward player center
                float dx = (P->x + 50) - (A->x + 50);
                float dy = (P->y + 100) - (A->y + 100);
                float dist = sqrtf(dx * dx + dy * dy);
                if (dist < 1.0f)
                    dist = 1.0f;

                arrows[i].x = A->x + 50;
                arrows[i].y = A->y + 100;
                arrows[i].vx = (dx / dist) * 2000.0f;
                arrows[i].vy = (dy / dist) * 2000.0f;
                arrows[i].timer = 3.0f;
                arrows[i].alive = true;
                break;
            }
        }
    }

    if (attackcheck)
    {
        if (CheckCollisionRecs(*AttackRect, ArcherRect))
        {
            A->health -= P->damage;
            A->knockbackduration = .01f;
        }
        if (A->knockbackduration > 0)
        {
            P->x -= 3000 * P->dashflag * dt;
            A->x += 3000 * P->dashflag * dt;
            A->knockbackduration -= dt;
        }
    }
    return attackcheckArcher;
}

void UpdateArrows(Arrow *arrows, int count, Player *P, float dt)
{
    for (int i = 0; i < count; i++)
    {
        if (!arrows[i].alive)
            continue;
        arrows[i].x += arrows[i].vx * dt;
        arrows[i].y += arrows[i].vy * dt;
        arrows[i].timer -= dt;

        if (arrows[i].timer <= 0)
        {
            arrows[i].alive = false;
            continue;
        }
        Rectangle arrowRect = {arrows[i].x - 20, arrows[i].y - 20, 40, 40};
        Rectangle playerRect = {P->x, P->y, 100, 200};
        if (CheckCollisionRecs(arrowRect, playerRect) && P->dashing == false)
        {
            if (P->iframes <= 0)
            {
                float overlapright = fabsf(P->x - (arrows[i].x + 20));
                float overlapleft = fabsf((arrows[i].x - 20) - (P->x + 100));
                float overlaptop = fabsf((P->y + 200) - (arrows[i].y - 20));
                float overlapbottom = fabsf((arrows[i].y + 20) - P->y);

                float minOverlap = fminf(fminf(overlapleft, overlapright), fminf(overlaptop, overlapbottom));

                if (minOverlap == overlapleft)
                {
                    P->spikeknkdirection = -1;
                    P->spikeknkbacktimer = 0.12f;
                }
                if (minOverlap == overlapright)
                {
                    P->spikeknkdirection = 1;
                    P->spikeknkbacktimer = 0.12f;
                }
                if (minOverlap == overlaptop)
                    P->velocityY = 1200.0f;
                if (minOverlap == overlapbottom)
                    P->velocityY = -1200.0f;
                P->health -= 15;
                P->iframes = invincibility;
            }
            arrows[i].alive = false;
        }
    }
}