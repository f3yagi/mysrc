#include "engine.h"
#include "action.h"
#include "conf.h"

int DomainCheck(Map *map, Object *o)
{
    float x0, y0, x1, y1;
    x0 = map->x;
    y0 = map->y;
    x1 = x0 + ENGINE_WIDTH;
    y1 = y0 + ENGINE_HEIGTH;
    if (o->x < x0 && o->x > x1 && o->y < y0 && o->y > y1) {
        return 1;
    }
    return 0;
}    

int IsHit(Object *o1, Object *o2)
{
    float x0 = X0(o1);
    float y0 = Y0(o1);
    float x1 = X1(o1);
    float y1 = Y1(o1);
    /*--------------*/
    float x2 = X0(o2);
    float y2 = Y0(o2);
    float x3 = X1(o2);
    float y3 = Y1(o2);
    return (x0 < x3 && x2 < x1 && y0 < y3 && y2 < y1) ? 1 : 0;
}

void DrawBase(Map *map, int texture, Object *o)
{
    float x, y;
    
    if (DomainCheck(map, o)) {
        return;
    }
#if DRAW_HIT_POINT
    DrawTextureClear();
    x = o->x - map->x;
    y = o->y - map->y;
    DrawLineHL(y, x, x + o->width, 0, 0, 0);
    DrawLineHL(y + o->height, x, x + o->width, 0, 0, 0);
    DrawLineVL(x, y, y + o->height, 0, 0, 0);
    DrawLineVL(x + o->width, y, y + o->height, 0, 0, 0);
#endif
    x = o->x - o->hit_x - map->x;
    y = o->y - o->hit_y - map->y;
#if DRAW_HIT_POINT     
    DrawLineHL(y, x, x + o->hit_width, 255, 0, 0);
    DrawLineHL(y + o->hit_height, x, x + o->hit_width, 255, 0, 0);
    DrawLineVL(x, y, y + o->hit_height, 255, 0, 0);
    DrawLineVL(x + o->hit_width, y, y + o->hit_height, 255, 0, 0);
#endif    
    DrawTextureObject(texture, x, y);
}    

int IsLeftWall(Map *map, Object *o)
{
    float x0 = X0(o);
    float y0 = Y0(o);
    float y1 = Y1(o);
    float cy = (y1 - y0) / 2.0f;
    
    if (IsWall(map, FIELD_X(x0), FIELD_Y(y0)) ||
        IsWall(map, FIELD_X(x0), FIELD_Y(cy)) ||
        IsWall(map, FIELD_X(x0), FIELD_Y(y1))) {
        return 1;
    }
    return 0;
}

int IsRightWall(Map *map, Object *o)
{
    float x1 = X1(o);
    float y0 = Y0(o);
    float y1 = Y1(o);
    float cy = (y1 - y0) / 2.0f;

    if (IsWall(map, FIELD_X(x1),  FIELD_Y(y0)) ||
        IsWall(map, FIELD_X(x1),  FIELD_Y(cy)) ||
        IsWall(map, FIELD_X(x1),  FIELD_Y(y1))) {
        return 1;
    }
    return 0;
}

int IsTopWall(Map *map, Object *o)
{
    float x0 = X0(o);
    float x1 = X1(o);
    float y0 = Y0(o);
    
    if (IsWall(map, FIELD_X(x0), FIELD_Y(y0)) ||
        IsWall(map, FIELD_X(x1), FIELD_Y(y0))) {
        return 1;
    }
    return 0;
}

int IsBottomWall(Map *map, Object *o)
{
    float x0 = X0(o);
    float x1 = X1(o);
    float y1 = Y1(o);
    
    if (IsWall(map, FIELD_X(x0), FIELD_Y(y1)) ||
        IsWall(map, FIELD_X(x1), FIELD_Y(y1))) {
        return 1;
    }
    return 0;
}

int IsTopWallJump(Map *map, Object *o)
{
    float x0 = X0(o);
    float x1 = X1(o);
    float y0 = Y0(o);
    
    if (IsWall(map, FIELD_X(x0), FIELD_Y(y0) - 1) ||
        IsWall(map, FIELD_X(x1), FIELD_Y(y0) - 1)) {
        return 1;
    }
    return 0;
}

int IsBottomWallJump(Map *map, Object *o)
{
    float x0 = X0(o);
    float x1 = X1(o);
    float y1 = Y1(o);
    
    if (IsWall(map, FIELD_X(x0), FIELD_Y(y1) + 1) ||
        IsWall(map, FIELD_X(x1), FIELD_Y(y1) + 1)) {
        return 1;
    }
    return 0;
}    


