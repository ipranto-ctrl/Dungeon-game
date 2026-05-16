#include "player.h"
#include "tilemap.h"
#include "enemies.h"

void CollisionX(Player *P)
{
    int tileX = (int)(P->x / TILE_SIZE);
    int tileY = (int)(P->y / TILE_SIZE);

    for (int i = tileY - 1; i <= tileY + 2; i++)
    {
        for (int j = tileX - 1; j <= tileX + 2; j++)
        {
            if (i < 0 || i >= MAP_ROWS || j < 0 || j >= MAP_COLS)
                continue;
            if (map[i][j] == 1)
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
            if (map[i][j] == 1)
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
        }
    }
}