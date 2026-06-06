#include "player.h"
#include "tilemap.h"
#include "enemies.h"

void CollisionX(Player *P, float dt)
{
    int tileX = (int)(P->x / TILE_SIZE);
    int tileY = (int)(P->y / TILE_SIZE);

    if (P->spikeknkbacktimer <= 0)
    {
        P->spikeknkdirection = 0;
        P->spikeknkbacktimer = 0;
    }
    P->x += P->spikeknkdirection * 1500 * dt;
    P->spikeknkbacktimer -= dt;

    for (int i = tileY - 1; i <= tileY + 2; i++)
    {
        for (int j = tileX - 1; j <= tileX + 2; j++)
        {
            if (i < 0 || i >= MAP_ROWS || j < 0 || j >= MAP_COLS) // wall
                continue;
            if (maps[currentLevel][i][j] == 1)
            {
                Rectangle tileRect = {j * TILE_SIZE, i * TILE_SIZE, TILE_SIZE, TILE_SIZE};
                Rectangle playerRect = {P->x, P->y, 100, 200};
                if (CheckCollisionRecs(playerRect, tileRect))
                {
                    float overlapLeft = (P->x + 100) - tileRect.x;
                    float overlapRight = (tileRect.x + tileRect.width) - P->x;
                    if (overlapLeft < overlapRight)
                        P->x = tileRect.x - 100;
                    else
                        P->x = tileRect.x + tileRect.width;
                }
            }
            if (maps[currentLevel][i][j] == 3) // spike
            {
                Rectangle spikeRect = {j * TILE_SIZE, i * TILE_SIZE, TILE_SIZE, TILE_SIZE};
                Rectangle playerRect = {P->x, P->y, 100, 200};
                if (CheckCollisionRecs(playerRect, spikeRect))
                {
                    if (P->iframes <= 0)
                    {
                        P->health -= 10;
                        P->iframes = invincibility;
                    }
                    P->spikeknkbacktimer = .25f;
                    float overlapLeft = (P->x + 100) - spikeRect.x;
                    float overlapRight = (spikeRect.x + spikeRect.width) - P->x;
                    if (overlapLeft < overlapRight)
                    {
                        P->spikeknkdirection = -1;
                        P->dashing = false;
                        P->dashtimer = 0.15f; // match the normal end-of-dash reset
                        // P->dashcooldown += 0.25f;
                    }
                    else
                    {
                        P->spikeknkdirection = 1;
                        P->dashing = false;
                        P->dashtimer = 0.15f; // match the normal end-of-dash reset
                        // P->dashcooldown += 0.25f;
                    }
                }
            }
        }
    }
}

void CollisionY(Player *P)
{
    int tileX = (int)(P->x / TILE_SIZE);
    int tileY = (int)(P->y / TILE_SIZE);

    for (int i = tileY - 1; i <= tileY + 2; i++)
    {
        for (int j = tileX - 1; j <= tileX + 2; j++)
        {
            if (i < 0 || i >= MAP_ROWS || j < 0 || j >= MAP_COLS)
                continue;
            if (maps[currentLevel][i][j] == 1)
            {
                Rectangle tileRect = {j * TILE_SIZE, i * TILE_SIZE, TILE_SIZE, TILE_SIZE};
                Rectangle playerRect = {P->x, P->y, 100, 200};
                if (CheckCollisionRecs(tileRect, playerRect))
                {
                    float overlaptop = (P->y + 200) - tileRect.y;
                    float overlapbottom = (tileRect.y + tileRect.height) - P->y;
                    if (overlaptop < overlapbottom)
                    {
                        P->y = tileRect.y - 200;
                        P->velocityY = 0;
                        P->onground = true;
                        P->doublejump = true;
                    }
                    else
                    {
                        P->y = tileRect.y + tileRect.height;
                        P->velocityY = 0;
                    }
                }
            }
            if (maps[currentLevel][i][j] == 3)
            {
                Rectangle spikeRect = {j * TILE_SIZE, i * TILE_SIZE, TILE_SIZE, TILE_SIZE};
                Rectangle playerRect = {P->x, P->y, 100, 200};
                if (CheckCollisionRecs(playerRect, spikeRect))
                {
                    if (P->iframes <= 0)
                    {
                        P->health -= 10;
                        P->iframes = invincibility;
                    }
                    P->velocityY = -3000;
                    P->doublejump = true;
                    P->x -= 10;
                }
            }
        }
    }
}