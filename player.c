#include "player.h"
#include "enemies.h"

void UpdateMovementX(Player *P, float dt)
{
    if (IsKeyDown(KEY_A) && !P->dashing && !IsKeyDown(KEY_D))
    {
        P->x -= P->speed * dt;
        P->dashflag = -1;
    }
    if (IsKeyDown(KEY_D) && !P->dashing && !IsKeyDown(KEY_A))
    {

        P->x += P->speed * dt;
        P->dashflag = 1;
    }
}

void UpdateDash(Player *P, float dt)
{
    if (IsKeyDown(KEY_LEFT_SHIFT) && !P->dashing && P->dashcooldown <= 0)
    {
        P->dashing = true;
    }
    if (P->dashing)
    {
        P->x += P->dashflag * 5000 * dt;
        P->dashtimer -= dt;
        if (P->dashtimer <= 0)
        {
            P->dashing = false;
            P->dashcooldown = 1.0f;
            P->dashtimer = 0.15f;
        }
    }
    P->dashcooldown -= dt;
    if (P->dashcooldown < 0)
        P->dashcooldown = 0;
}

void UpdateJump(Player *P, float dt)
{
    // jump starts
    if (P->onground && IsKeyPressed(KEY_SPACE))
    {
        P->velocityY = -3200.0f;
        P->onground = false;
    }
    else if (!P->onground && P->doublejump && IsKeyPressed(KEY_SPACE))
    {
        P->velocityY = -3200.0f;
        P->doublejump = false;
    }
    if(IsKeyReleased(KEY_SPACE) && P->velocityY<0)
    {
        P->velocityY*=0.15f;
    }

    // jump ends
}

void UpdateGravity(Player *P, float dt)
{
    // Y movement starts
    P->velocityY += P->gravity * dt;
    P->y += P->velocityY * dt;
    P->onground = false;
    // y movement ends
}
int UpdateAttack(Player *P, float dt, Rectangle *AttackRect)
{
    P->attackcooldown -= dt;
    if (P->attackcooldown <= 0)
    {
        P->attackcooldown = 0;
    }
    if (IsKeyPressed(KEY_BACKSPACE) && P->attackcooldown <= 0)
    {
        P->attackcooldown = .25;
        if (IsKeyDown(KEY_W))
        {
            AttackRect->x = P->x - 100;
            AttackRect->y = P->y-400;
            AttackRect->height = 300;
            AttackRect->width = 300;
        }
        else if (P->dashflag == -1)
        {
            AttackRect->x = P->x - 300;
            AttackRect->y = P->y;
            AttackRect->height = 200;
            AttackRect->width = 300;
        }
        else if (P->dashflag == 1)
        {
            AttackRect->x = P->x + 100;
            AttackRect->y = P->y;
            AttackRect->height = 200;
            AttackRect->width = 300;
        }
        return 1;
    }
    else
        return 0;
}