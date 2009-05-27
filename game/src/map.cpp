#include <stdio.h>
#include "lib.h"
#include "engine.h"
#include "res.h"
#include "action.h"

/* lib.cpp */
FILE *FOpen(char *name, char *mode);
void FClose(FILE *fp);

#define POS(x, y) (map->width * (y) + (x))

Map *ReadMap(char *name)
{
    char *buf;
    int x, y, i, n;
    Map *map = NULL;
    char *data;

    buf = LoadFile(name, &n);
    data = buf;
    map = (Map *)Malloc(sizeof(Map));
    map->object = NULL;
    MemCpy(&map->width, buf, sizeof(int));
    buf += sizeof(int);
    MemCpy(&map->height, buf, sizeof(int));
    buf += sizeof(int);
    map->data = (struct MapData *)Malloc(sizeof(struct MapData) * map->width * map->height);
    for (y = 0; y < map->height; y++) {
        for (x = 0; x < map->width; x++) {
            MemCpy(&map->data[POS(x, y)], buf, sizeof(struct MapData));
            buf += sizeof(struct MapData);
        }
    }
    MemCpy(&map->objectNum, buf, sizeof(int));
    buf += sizeof(int);
    map->object = (struct ObjectData *)Malloc(sizeof(struct ObjectData) * map->objectNum);    
    for (i = 0; i < map->objectNum; i++) {
        MemCpy(&map->object[i], buf, sizeof(struct ObjectData));
        buf += sizeof(struct ObjectData);
    }
    Free(data);
    return map;
}

void InitMap(Map *map)
{
    map->x = 0;
    map->y = 0;
}    

void FreeMap(Map *map)
{
    if (map->object != NULL) {
        Free(map->object);
    }
    Free(map);
}

void MoveMap(Map *map)
{
    Object *o = TO_OBJECT(GetPlayer());
    float x = CX(o);
    float y = CY(o);
    float width  = (float)ENGINE_WIDTH;
    float height = (float)ENGINE_HEIGTH;
    
    if (x + width / 2.0f > map->width * BLOCK_WIDTH) {
        map->x = map->width * BLOCK_WIDTH - width;
    } 
    if (x - width / 2.0f >= 0 &&
        x + width / 2.0f < map->width * BLOCK_WIDTH) {
        map->x = x - width / 2.0f;
    }
/*    
    if (y + height / 2.0f > map->height * BLOCK_HEIGHT) {
        map->y = map->height * BLOCK_HEIGHT - height;
    }
    if (y - height / 2.0f >= 0 &&
        y + height / 2.0f < map->height * BLOCK_HEIGHT) {
        map->y = y - height / 2.0f;
    }
*/    
}    

int IsWall(Map *map, int x, int y)
{
    if (0 > x || x >= map->width || 0 > y || y >= map->height) {
        return 0;
    }
    return map->data[POS(x, y)].wall;
}    

void CreateObject(Map *map)
{
    int i;

    InitEnemy();
    InitItem();
    InitBomb();
    InitEffect();
    for (i = 0; i < map->objectNum; i++) {
        float x = (float)map->object[i].x * BLOCK_WIDTH;
        float y = (float)map->object[i].y * BLOCK_HEIGHT;
        char type = map->object[i].type;
        int r;
        if (type == PLAYER) {
            InitPlayer(x, y);
        }
        r = CreateEnemy(type, x, y);
        if (r) {
            continue;
        }
        CreateItem(type, x, y);
    }
    MoveMap(map);
}    
