#include <stdio.h>
#include "engine.h"
#include "lib.h"
#include "action.h"
#include "res.h"

#define MAX_ENEMY 256
static Enemy enemy[MAX_ENEMY];
static int enemySize = 0;

void InitEnemy()
{
    enemySize = 0;
}    

void MoveEnemy(Map *map)
{
    int i;
    
    for (i = 0; i < enemySize; i++) {
        if (!enemy[i].die &&
            !DomainCheck(map, TO_OBJECT(&enemy[i]))) {
            enemy[i].Move(map, &enemy[i]);
        }
    }
}

void DrawEnemy(Map *map)
{
    int i;

    for (i = 0; i < enemySize; i++) {
        if (!enemy[i].die &&
            !DomainCheck(map, TO_OBJECT(&enemy[i]))) {
            enemy[i].Draw(map, &enemy[i]);
        }
    }
}

static int iterPtr = 0;

void InitEnemyIter()
{
    iterPtr = 0;
}    

Enemy *EnemyIter(Map *map)
{
    while (iterPtr < enemySize) {
        Enemy *e = &enemy[iterPtr];
        if (e->die == 0 && DomainCheck(map, TO_OBJECT(e)) == 0) {
            iterPtr++;
            return e;
        }
        iterPtr++;
    }
    return NULL;
}

void MoveEnemy(Map *map, Enemy *enemy)
{
    Object *o = TO_OBJECT(enemy);
    
    /* left */
    if (IsLeftWall(map, o)) {
        X0(o) = (float)(FIELD_X(X0(o)) + 1) * BLOCK_WIDTH;
        enemy->data[0] = 1;
    }
    /* right */
    if (IsRightWall(map, o)) {
        X0(o) = (float)(FIELD_X(X0(o)) + 1) * BLOCK_WIDTH - o->hit_width;
        enemy->data[0] = 0;
    }
    if (!enemy->jump) {
        if (!IsTopWallJump(map, o)) {
            enemy->jump = 1;
        }
        if (!IsBottomWallJump(map, o)) {
            enemy->jump = 1;
        }
    } else {
        const float G = 0.20f;        
        enemy->vy += G;
        Y0(o) += enemy->vy;
        if (enemy->vy > 10) {
            enemy->vy = 10;
        }
        /* up */
        if (IsTopWall(map, o)) {
            Y0(o) = (float)(FIELD_Y(Y0(o)) + 1) * BLOCK_HEIGHT;
            enemy->vy = 0;
        }
        /* down */
        if (IsBottomWall(map, o)) {
            Y0(o) = (float)(FIELD_Y(Y1(o)) - 1) * BLOCK_HEIGHT;
            enemy->jump = 0;
        }
    }
    if (Y1(o) > map->height * BLOCK_HEIGHT) {
        enemy->die = 1;
    }
    if (X0(o) < 0) {
        enemy->die = 1;
    }
}

static void MoveHoge(Map *map, Enemy *enemy)
{
    Object *o = TO_OBJECT(enemy);
    Player *player = GetPlayer();
    Object *p = TO_OBJECT(player);
    
    if (player->jump &&
        player->vy > 0 &&
        Fabs(CX(o) - CX(p)) < o->hit_width &&
        Fabs(CY(o) - CY(p)) < o->hit_height) {
        enemy->die = 1;
        score += 10;
        player->vy = -4.0f;
        if (PushKey(KEY_X)) {
            player->vy -= 4.0f;
        }
        return;
    }
    if (enemy->data[0]) {
        o->x += 0.8f;
    } else {
        o->x -= 0.8f;
    }
    MoveEnemy(map, enemy);
    if (IsHit(TO_OBJECT(player), o)) {
        player->die = 1;
    }
}    

static void DrawHoge(Map *map, Enemy *enemy)
{
    DrawBase(map, res.enemy_bird, TO_OBJECT(enemy));
}

static void DrawDog(Map *map, Enemy *enemy)
{
    DrawBase(map, res.enemy_dog, TO_OBJECT(enemy));
}

static void EnemyBird(Enemy *enemy, float x, float y)
{
    TO_OBJECT(enemy)->x = x;
    TO_OBJECT(enemy)->y = y;
    TO_OBJECT(enemy)->width  = 32;
    TO_OBJECT(enemy)->height = 32;
    
    TO_OBJECT(enemy)->hit_x = 5;
    TO_OBJECT(enemy)->hit_y = 0;
    TO_OBJECT(enemy)->hit_width  = 28;
    TO_OBJECT(enemy)->hit_height = 32;    
    
    enemy->Move = MoveHoge;
    enemy->Draw = DrawHoge;
    enemy->die  = 0;
    enemy->data[0]  = 0;
}    

static void EnemyDog(Enemy *enemy, float x, float y)
{
    TO_OBJECT(enemy)->x = x;
    TO_OBJECT(enemy)->y = y;
    TO_OBJECT(enemy)->width  = 30;
    TO_OBJECT(enemy)->height = 32;

    TO_OBJECT(enemy)->hit_x = 3;
    TO_OBJECT(enemy)->hit_y = 0;
    TO_OBJECT(enemy)->hit_width  = 32;
    TO_OBJECT(enemy)->hit_height = 32;    
    
    enemy->Move = MoveHoge;
    enemy->Draw = DrawDog;
    enemy->die  = 0;
    enemy->data[0]  = 0;
}    

int CreateEnemy(char type, float x, float y)
{
    switch (type) {
    case ENEMY_BIRD:
        EnemyBird(&enemy[enemySize], x, y);
        goto end;
        break;

    case ENEMY_DOG:
        EnemyDog(&enemy[enemySize], x, y);
        goto end;
        break;
    }
    return 0;
 end:
    enemySize++;
    return 1;
}    
