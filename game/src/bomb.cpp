#include <stdio.h>
#include "engine.h"
#include "action.h"
#include "res.h"

#define MAX_BOMB 256
static Bomb bomb[MAX_BOMB];

void InitBomb()
{
    int i;
    
    for (i = 0; i < MAX_BOMB; i++) {
        bomb[i].die = 1;
    }
}

void MoveBomb(Map *map)
{
    int i;
    
    for (i = 0; i < MAX_BOMB; i++) {
        if (!bomb[i].die) {
            bomb[i].Move(map, &bomb[i]);
        }
    }
}

void DrawBomb(Map *map)
{
    int i;
    
    for (i = 0; i < MAX_BOMB; i++) {
        if (!bomb[i].die) {
            bomb[i].Draw(map, &bomb[i]);
        }
    }
}    

static void BombMoveBase(Map *map, Bomb *bomb)
{
    Object *o =  TO_OBJECT(bomb);

    switch (bomb->dir) {
    case DIR_RIGHT:
        X0(o) += 3;
        break;

    case DIR_LEFT:
        X0(o) -= 3;
        break;

    case DIR_TOP:
        Y0(o) -= 3;
        break;
    }
    bomb->distance += 3;
    InitEnemyIter();
    for ( ; ; ) {
        Enemy *enemy = EnemyIter(map);
        if (enemy == NULL) {
            break;
        }
        if (IsHit(o, TO_OBJECT(enemy))) {
            enemy->die = 1;
            bomb->die = 1;
            CreateEffect(EFFECT_EFFECT1, X0(o), Y0(o));
        }
    }
    if (IsRightWall(map, o) ||
        IsLeftWall(map, o)  ||
        DomainCheck(map, o) ||
        bomb->distance > 100) {
        bomb->die = 1;
    }
}    

static void DrawBombApple(Map *map, Bomb *bomb)
{
    DrawBase(map, res.item_apple, TO_OBJECT(bomb));    
}

static void BombApple(Bomb *bomb, enum Direction dir, float x, float y)
{
    TO_OBJECT(bomb)->x = x;
    TO_OBJECT(bomb)->y = y;
    TO_OBJECT(bomb)->width  = 32;
    TO_OBJECT(bomb)->height = 32;

    TO_OBJECT(bomb)->hit_x = 0;
    TO_OBJECT(bomb)->hit_y = 0;
    TO_OBJECT(bomb)->hit_width  = 32;
    TO_OBJECT(bomb)->hit_height = 32;

    bomb->dir      = dir;
    bomb->distance = 0;
    bomb->Move = BombMoveBase;
    bomb->Draw = DrawBombApple;
    bomb->die  = 0;
}

static Bomb *BombNew()
{
    int i;

    for (i = 0; i < MAX_BOMB; i++) {
        if (bomb[i].die) {
            return &bomb[i];
        }
    }
    return NULL;
}    

void CreateBomb()
{
    float x, y;
    Player *player = GetPlayer();
    Object *o = TO_OBJECT(player);
    Bomb *bomb = BombNew();

    switch (player->dir) {
    case DIR_RIGHT:
        x = X1(o);
        y = Y0(o);
        break;

    case DIR_LEFT:
        x = X0(o) - o->hit_width;
        y = Y0(o);
        break;

    case DIR_TOP:
        x = X0(o);
        y = Y0(o) - o->hit_height;
        break;
    }
    if (bomb != NULL) {
        BombApple(bomb, player->dir, x, y);
    }
}    
