#define _USE_MATH_DEFINES
#include "engine.h"
#include "action.h"
#include "res.h"
#include "lib.h"

static Player player;

static void SetPlayer(Player *player, float x, float y)
{
    /* hit pos */
    TO_OBJECT(player)->hit_x  = 4;
    TO_OBJECT(player)->hit_y  = 0;
    TO_OBJECT(player)->hit_width  = 25;
    TO_OBJECT(player)->hit_height = 32;
    /* pos */    
    TO_OBJECT(player)->x = x;
    TO_OBJECT(player)->y = y;
    TO_OBJECT(player)->width  = 32;
    TO_OBJECT(player)->height = 32;
    /* flag */
    player->dir   = DIR_RIGHT;
    player->jump  = 0;
    player->die   = 0;
    player->clear = 0;
}

void InitPlayer(float x, float y)
{
    SetPlayer(&player, x, y);
}

Player *GetPlayer()
{
    return &player;
}

void IsNextStage()
{
    player.clear = 1;
}    

int IsPlayerClear()
{
    return player.clear;
}

int IsPlayerDie()
{
    return player.die;
}    

static int jumpKey = 0;

int PushJumpKey()
{
    if (PushKey(KEY_X) == 1 && jumpKey == 0) {
        jumpKey = 1;
        return 1;
    } 
    if (PushKey(KEY_X) == 0 && jumpKey == 1) {
        jumpKey = 0;
        return 0;
    }
    return 0;
}

static int shotKey = 0;

int PushShotKey()
{
    if (PushKey(KEY_Z) == 1 && shotKey == 0) {
        shotKey = 1;
        return 1;
    } 
    if (PushKey(KEY_Z) == 0 && shotKey == 1) {
        shotKey = 0;
        return 0;
    }
    return 0;
}

void MovePlayer(Map *map)
{
    Player *player = GetPlayer();
    Object *o = TO_OBJECT(player);

    if (PushKey(KEY_LEFT)) {
        player->dir = DIR_LEFT;
        X0(o) -= 2;
    }
    if (PushKey(KEY_RIGHT)) {
        player->dir = DIR_RIGHT;
        X0(o) += 2;
    }
    if (PushKey(KEY_UP)) {
        player->dir = DIR_TOP;
    }
    /* left */
    if (IsLeftWall(map, o)) {
        X0(o) = (float)(FIELD_X(X0(o)) + 1) * BLOCK_WIDTH;
    }
    /* right */
    if (IsRightWall(map, o)) {
        X0(o) = (float)(FIELD_X(X0(o)) + 1) * BLOCK_WIDTH - o->hit_width;
    }
    if (!player->jump) {
        if (PushJumpKey() && !IsTopWallJump(map, o)) {
            SoundPlay(res.sound_jump);
            player->vy   = - 6.0f;
            player->jump = 1;
        }
        if (!IsBottomWallJump(map, o)) {
            player->jump = 1;
        }
    } else {
        const float G = 0.20f;        
        player->vy += G;
        Y0(o) += player->vy;
        if (player->vy > 10) {
            player->vy = 10;
        }
        /* up */
        if (IsTopWall(map, o)) {
            Y0(o) = (float)(FIELD_Y(Y0(o)) + 1) * BLOCK_HEIGHT;
            player->vy = 0;
        }
        /* down */
        if (IsBottomWall(map, o)) {
            Y0(o) = (float)(FIELD_Y(Y1(o)) - 1) * BLOCK_WIDTH;
            player->jump = 0;
        }
    }
    if (PushShotKey()) {
        CreateBomb();
    }
    if (Y1(o) > map->height * BLOCK_HEIGHT) {
        player->die = 1;
    }
    if (X0(o) > map->width * BLOCK_WIDTH) {
        player->clear = 1;
    }
    if (X0(o) < 0) {
        X0(o) = 0;
    }
}

void DrawPlayer(Map *map)
{
    Player *player = GetPlayer();

    switch (player->dir) {
    case DIR_RIGHT:
        DrawBase(map, res.player_right, TO_OBJECT(player));
        break;

    case DIR_LEFT:
        DrawBase(map, res.player_left, TO_OBJECT(player));
        break;

    default:
        DrawBase(map, res.player_right, TO_OBJECT(player));    
        break;
    }
/* draw center pos */    
#if 0
    x0 = 320;
    y0 = 0;
    x1 = 321;
    y1 = 480;
    DrawVertex(x0, y0, x1, y1, 0, 0, 1, 1, 250, 250, 250);
#endif    
}    

