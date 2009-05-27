#include <stdio.h>
#include "lib.h"
#include "engine.h"
#include "action.h"
#include "res.h"

/* lib.cpp */
FILE *FOpen(char *name, char *mode);
void FClose(FILE *fp);

#define POS(x, y) (edit->width * (y) + (x))

static char playerOn = 0;
static Map *edit = NULL;
static char editName[MAX_PATH];

static struct Map *CreateMap(int width, int height)
{
    int i, size;
    Map *edit;

    edit = (struct Map*)Malloc(sizeof(struct Map));
    edit->x = 0;
    edit->y = 0;
    edit->width  = width;
    edit->height = height;
    size = edit->width * edit->height;
    edit->data = (struct MapData *)Malloc(sizeof(struct MapData) * size);
    edit->object = (struct ObjectData *)Malloc(sizeof(struct ObjectData) * size);
    for (i = 0; i < size; i++) {
        edit->data[i].wall  = 0;
        edit->data[i].block = BLOCK_NODEF;
        edit->object[i].type = OBJECT_NODEF;
    }
    return edit;
}

static void WriteMap(struct Map *edit, char *name);

static void ResizeMap(int width, int height)
{
    int x, y;
    Map *editNew;
    int tmp_width, tmp_height;
    
    editNew = CreateMap(width, height);
    if (width > edit->width) {
        tmp_width  = edit->width;
        tmp_height = edit->height;
    } else {
        tmp_width  = width;
        tmp_height = height;
    }
    for (y = 0; y < tmp_height; y++) {
        for (x = 0; x < tmp_width; x++) {
            editNew->data[width * y + x]    = edit->data[edit->width * y + x];
            editNew->object[width * y + x]  = edit->object[edit->width * y + x];
        }
    }
    WriteMap(editNew, "resize.map");
}

Map *OpenMapFile(char *name)
{
    FILE *fp;
    int x, y, i, objectNum;
    Map *edit;

    fp = FOpen(name, "rb");
    if (fp == NULL) {
        EngineError("not find %s\n", name);
    }
    StrCpy(editName, MAX_PATH, name);
    edit = (Map *)Malloc(sizeof(Map));
    fread(&edit->width, sizeof(int), 1, fp);
    fread(&edit->height, sizeof(int), 1, fp);
    WriteLog("map %d %d\n", edit->width, edit->height);
    edit->data = (struct MapData *)Malloc(sizeof(struct MapData) * edit->width * edit->height);
    for (y = 0; y < edit->height; y++) {
        for (x = 0; x < edit->width; x++) {
            fread(&edit->data[POS(x, y)], sizeof(struct MapData), 1, fp);
        }
    }
    edit->object = (struct ObjectData *)Malloc(sizeof(struct ObjectData) * edit->width * edit->height);
    for (i = 0; i < edit->height * edit->width; i++) {
        edit->object[i].type = OBJECT_NODEF;
    }
    fread(&objectNum, sizeof(int), 1, fp);
    WriteLog("objectNum %d\n", objectNum);
    for (i = 0; i < objectNum; i++) {
        struct ObjectData object;
        fread(&object, sizeof(struct ObjectData), 1, fp);
        edit->object[POS(object.x, object.y)].type = object.type;
        if (edit->object[POS(object.x, object.y)].type == PLAYER) {
            playerOn = 1;
        }
    }
    edit->x = 0;
    edit->y = 0;
    FClose(fp);
    return edit;
}

static void QuitMapWriter();

Map *InitMapWriter()
{
    FILE *fp;
    char name[MAX_PATH];
    int width;

    AddFinalize(QuitMapWriter);
    if (OpenFileDialog(name, "map [*.map]\0*.map\0" "All files [*.*]\0*.*\0")) { 
        return OpenMapFile(name);
    }
    StrCpy(editName, MAX_PATH, "new.map");
    fp = FOpen("map_conf.txt", "r");
    if (fp != NULL) {
        fgets(name, MAX_PATH, fp);
        FClose(fp);
        width = Atoi(name);
        if (0 > width) {
            EngineError("map width error\n", width);
        }
    } else {
        width = 5;
    }
    return CreateMap(ENGINE_WIDTH / BLOCK_WIDTH * width, ENGINE_HEIGTH / BLOCK_HEIGHT);
}

static void WriteMap(Map *edit, char *name)
{
    int x, y, objectNum = 0;
    FILE *fp;

    fp = FOpen(name, "wb");
    fwrite(&edit->width, sizeof(int), 1, fp);
    fwrite(&edit->height, sizeof(int), 1, fp);
    for (y = 0; y < edit->height; y++) {
        for (x = 0; x < edit->width; x++) {
            fwrite(&edit->data[POS(x, y)],  sizeof(struct MapData), 1, fp);
        }
    }
    for (y = 0; y < edit->height; y++) {
        for (x = 0; x < edit->width; x++) {
            if (edit->object[POS(x, y)].type != OBJECT_NODEF) {
                objectNum++;
            }
        }
    }
    fwrite(&objectNum, sizeof(int), 1, fp);
    for (y = 0; y < edit->height; y++) {
        for (x = 0; x < edit->width; x++) {
            if (edit->object[POS(x, y)].type != OBJECT_NODEF) {
                struct ObjectData object;
                object.x = x;
                object.y = y;
                object.type = edit->object[POS(x, y)].type;                
                fwrite(&object, sizeof(struct ObjectData), 1, fp);
            }
        }
    }
    FClose(fp);
}

static void QuitMapWriter()
{
    if (!edit) return;
    Free(edit->data);
    Free(edit->object);
    Free(edit);
}

struct Selector {
    char mapMode;
    float x, y; /* pixel */
    int block_x, block_y;
    int width, height;  /* block width height*/
    int type;           /* enum BlockType or ObjectType */
};
static struct Selector *selector = NULL;

static void InitKeyMapMode();

static Selector *SelectorNew()
{
    struct Selector *selector = NULL;
    selector = (struct Selector *)Malloc(sizeof(struct Selector));
    selector->mapMode  = 1;
    selector->x = 0;
    selector->y = 0;
    selector->block_x = 0;
    selector->block_y = 0;
    selector->width  = BLOCK_WIDTH;
    selector->height = BLOCK_HEIGHT;
    selector->type   = 1;
    InitKeyMapMode();
    return selector;
}

static void MoveMapSelector(Map *edit, struct Selector *selector)
{
    float nextX = (float)selector->block_x * BLOCK_WIDTH;
    float nextY = (float)selector->block_y * BLOCK_HEIGHT;

    if (selector->x != nextX || selector->y != nextY) {
        if (selector->x < nextX) {
            selector->x += 4;
        }
        if (selector->y < nextY) {
            selector->y += 4;
        }
        if (selector->x > nextX) {
            selector->x -= 4;
        }
        if (selector->y > nextY) {
            selector->y -= 4;
        }
    } else {
        if (PushKey(KEY_LEFT)) {
            if (selector->block_x - 1 >= 0) {
                selector->block_x--;
            }
        }
        if (PushKey(KEY_RIGHT)) {
            if (selector->block_x + 1 < edit->width) {
                selector->block_x++;
            }
        }
        if (PushKey(KEY_UP)) {
            if (selector->block_y - 1 >= 0) {
                selector->block_y--;
            }
        }
        if (PushKey(KEY_DOWN)) {
            if (selector->block_y + 1 < edit->height) {
                selector->block_y++;
            }
        }
    }
    if (PushKey(KEY_Z)) {
        /* set map */
        int x = selector->block_x + (int)edit->x / BLOCK_WIDTH;
        int y = selector->block_y;
        edit->data[POS(x, y)].block = selector->type;
        edit->data[POS(x, y)].wall = 1;
    }
    if (PushKey(KEY_C)) {
        /* delete map*/
        int x = selector->block_x + (int)edit->x / BLOCK_WIDTH;;
        int y = selector->block_y;
        edit->data[POS(x, y)].block = BLOCK_NODEF;
        edit->data[POS(x, y)].wall  = 0;
    }
#if 0    
    if (PushKey(KEY_F)) {
        /* wall 0 */
        int x = selector->block_x + (int)edit->x / BLOCK_WIDTH;
        int y = selector->block_y;
        edit->data[POS(x, y)].wall = 0;
    }
#endif    
}

static void MoveObjectSelector(Map *edit, struct Selector *selector)
{
    float nextX = (float)selector->block_x * BLOCK_WIDTH;
    float nextY = (float)selector->block_y * BLOCK_HEIGHT;

    if (selector->x != nextX || selector->y != nextY) {
        if (selector->x < nextX) {
            selector->x += 4;
        }
        if (selector->y < nextY) {
            selector->y += 4;
        }
        if (selector->x > nextX) {
            selector->x -= 4;
        }
        if (selector->y > nextY) {
            selector->y -= 4;
        }
    } else {
        if (PushKey(KEY_LEFT)) {
            if (selector->block_x - 1 >= 0) {
                selector->block_x--;
            }
        }
        if (PushKey(KEY_RIGHT)) {
            if (selector->block_x + 1 < edit->width) {
                selector->block_x++;
            }
        }
        if (PushKey(KEY_UP)) {
            if (selector->block_y - 1 >= 0) {
                selector->block_y--;
            }
        }
        if (PushKey(KEY_DOWN)) {
            if (selector->block_y + 1 < edit->height) {
                selector->block_y++;
            }
        }
    }
    if (PushKey(KEY_C)) {
        /* delete map*/
        int x = selector->block_x + (int)edit->x / BLOCK_WIDTH;
        int y = selector->block_y;
        if (edit->object[POS(x, y)].type == PLAYER) {
            playerOn = 0;
        }
        edit->object[POS(x, y)].type = OBJECT_NODEF;
    }
    if (PushKey(KEY_Z)) {
        int x = selector->block_x + (int)edit->x / BLOCK_WIDTH;
        int y = selector->block_y;
        if (selector->type == PLAYER && playerOn == 0) {
            edit->object[POS(x, y)].x = x;
            edit->object[POS(x, y)].y = y;
            edit->object[POS(x, y)].type = selector->type;
            playerOn = 1;
        } else if (selector->type != PLAYER && edit->data[POS(x, y)].wall == 0) {
            edit->object[POS(x, y)].x = x;
            edit->object[POS(x, y)].y = y;
            edit->object[POS(x, y)].type = selector->type;
        }
    }
}

/* key conf */
/*------------------------------------------------------------------*/
static void KeySelectNextMap()
{
    if (selector->type + 1 < BLOCK_END) {
        selector->type++;
    } else {
        selector->type = 1;
    }
}

static void KeySelectNextObject()
{
    if (selector->type + 1 < OBJECT_END) {
       selector->type++;
    } else {
        selector->type = PLAYER;
    }
}

static void KeyChangeWall()
{
    int x = selector->block_x + (int)edit->x / BLOCK_WIDTH;
    int y = selector->block_y;
    edit->data[POS(x, y)].wall ^= 1;
}    

static void ChangeMode()
{
    selector->mapMode ^= 1;
    if (selector->mapMode) {
        winput.KeyX = KeySelectNextMap;
        winput.KeyF = KeyChangeWall;
        selector->width  = BLOCK_WIDTH;
        selector->height = BLOCK_HEIGHT;
        selector->type   = 1;
    } else {
        winput.KeyX = KeySelectNextObject;
        winput.KeyF = NULL;
        selector->width  = BLOCK_WIDTH;
        selector->height = BLOCK_HEIGHT;
        selector->type   = PLAYER;
    }
}    

static void NextMap()
{
    while (edit->x + ENGINE_WIDTH < edit->width * BLOCK_WIDTH) {
        edit->x += BLOCK_WIDTH;
    }
}    

static void KeyNextMap()
{
    if (edit->x + ENGINE_WIDTH < edit->width * BLOCK_WIDTH) {
        edit->x += BLOCK_WIDTH;
    }
}

static void PrevMap()
{
    while (edit->x - BLOCK_WIDTH >= 0) {
        edit->x -= BLOCK_WIDTH;
    }
}

static void KeyPrevMap()
{
    if (edit->x - BLOCK_WIDTH >= 0) {
        edit->x -= BLOCK_WIDTH;
    }
}

static int PlayerObjectCheck(Map *edit)
{
    int x, y, playerOn = 0;
    for (y = 0; y < edit->height; y++) {
        for (x = 0; x < edit->width; x++) {
            if (edit->object[POS(x, y)].type == PLAYER) {
                playerOn = 1;
            }
        }
    }
    return playerOn;
}    

static void KeySaveMap()
{
    int playerOn = 0;
    char name[MAX_PATH];

    if (!PlayerObjectCheck(edit)) {
        PrintMessageBox("プレイヤーを配置して下さい");
        return;
    }
    if (SaveFileDialog(name, "map [*.map]\0*.map\0" "All files [*.*]\0*.*\0")) {
        WriteMap(edit, name);
        StrCpy(editName, MAX_PATH, name);
    } 
}

static void Exec();

static void InitKeyMapMode()
{
    /* common key */
    winput.KeyS = KeyPrevMap;
    winput.KeyD = KeyNextMap;
    winput.KeyQ = KeySaveMap;
    winput.KeyA = ChangeMode;
    winput.KeyT = Exec;
    /* map mode key */
    winput.KeyX = KeySelectNextMap;
    winput.KeyF = KeyChangeWall;
}
/*------------------------------------------------------------------*/
void DrawLineHL(float y, float begin, float end, byte r, byte g, byte b)
{
    float x0 = begin;
    float y0 = y;
    float x1 = end;
    float y1 = y + 1;
    DrawVertex(x0, y0, x1, y1, 0, 0, 1, 1, r, g, b);
}

void DrawLineVL(float x, float begin, float end, byte r, byte g, byte b)
{
    float x0 = x;
    float y0 = begin;
    float x1 = x + 1;
    float y1 = end;
    DrawVertex(x0, y0, x1, y1, 0, 0, 1, 1, r, g, b);
}

static void DrawLine()
{
    float x, y;

    DrawTextureClear();
    for (x = 0; x < ENGINE_WIDTH; x += BLOCK_WIDTH) {
        DrawLineVL(x, 0, ENGINE_HEIGTH, 0, 0, 0);
    }
    for (y = 0; y < ENGINE_HEIGTH; y += BLOCK_HEIGHT) {
        DrawLineHL(y, 0, ENGINE_WIDTH, 0, 0, 0);
    }
}

static void DrawObject(Map *edit)
{
    int x, y;
    int x0, y0, x1, y1;
    
    x0 = (int)edit->x / BLOCK_WIDTH;
    y0 = (int)edit->y / BLOCK_HEIGHT;
    x1 = x0 + ENGINE_WIDTH / BLOCK_WIDTH;
    y1 = y0 + ENGINE_HEIGTH / BLOCK_HEIGHT;
    
    for (y = y0; y <= y1; y++) {
        for (x = x0; x <= x1; x++) {
            float drawX0, drawY0;
            drawX0 = (float)BLOCK_WIDTH  * x - edit->x;
            drawY0 = (float)BLOCK_HEIGHT * y - edit->y;
            SwitchObject(edit->object[POS(x, y)].type, drawX0, drawY0);
        }
    }
}

static void DrawSelector(struct Selector *selector)
{
    float x0, y0, x1, y1;

    x0 = selector->x;
    y0 = selector->y;
    if (x0 + BLOCK_WIDTH > ENGINE_WIDTH) {
        x0 = ENGINE_WIDTH - BLOCK_WIDTH;
        selector->block_x = (int)x0 / BLOCK_WIDTH;
    }
    if (y0 + BLOCK_HEIGHT > ENGINE_HEIGTH) {
        y0 = ENGINE_HEIGTH - BLOCK_HEIGHT;
        selector->block_y = (int)y0 / BLOCK_HEIGHT;        
    }
    x1 = x0 + selector->width;
    y1 = y0 + selector->height;
    if (selector->mapMode) {
        SwitchBlock(selector->type, x0, y0);
    } else {
        SwitchObject(selector->type, x0, y0);
    }
    DrawTextureClear();
    DrawLineVL(x0 + BLOCK_WIDTH  / 2, y0, y1, 0, 0, 255);
    DrawLineHL(y0 + BLOCK_HEIGHT / 2, x0, x1, 0, 0, 255);
}

static int CheckValue(int x, int y, int mapWidth, int mapHeight, struct MapData *data)
{
    if (x < 0 || x > mapWidth || y < 0 || y > mapHeight) {
        return 0;
    }
    return 1;
}    

void DrawMap(Map *map, int editMode)
{
    int x, y;
    int x0, y0, x1, y1;
    
    x0 = (int)map->x / BLOCK_WIDTH;
    y0 = (int)map->y / BLOCK_HEIGHT;
    x1 = x0 + ENGINE_WIDTH / BLOCK_WIDTH;
    y1 = y0 + ENGINE_HEIGTH / BLOCK_HEIGHT;
    
    for (y = y0; y <= y1; y++) {
        for (x = x0; x <= x1; x++) {
            float drawX0, drawY0;
            if (!CheckValue(x, y, map->width, map->height, map->data)) {
                continue;
            }
            drawX0 = (float)BLOCK_WIDTH  * x - map->x;
            drawY0 = (float)BLOCK_HEIGHT * y - map->y;
            SwitchBlock(map->data[map->width * y + x].block, drawX0, drawY0);
            if (editMode && map->data[map->width * y + x].wall) {
                DrawTextureClear();
                DrawLineVL(drawX0 + 16, drawY0, drawY0 + 32, 255, 0, 0);
                DrawLineHL(drawY0 + 16, drawX0, drawX0 + 32, 255, 0, 0);
            }
        }
    }
}

static void MapWriterDraw()
{
    DrawBackGround(255, 255, 255);
    DrawMap(edit, 1);
    DrawObject(edit);
    DrawSelector(selector);
    DrawLine();
}

static int state = 0;
static int stageState = 0;

static void Exec()
{
    if (state == 3) {
        state = 1;
        StopSound();
    } else {
        WriteMap(edit, editName);
        state = 3;
        stageState = 0;
    }
}    

enum ActionMenu MapWritterMain()
{
    switch (state) {
    case 0:
        edit = InitMapWriter();
        selector = SelectorNew();
        ChangeMode();/* fot debug */
        
        state = 1;
        break;

    case 1:
        DrawMain = MapWriterDraw;
        state = 2;
        break;

    case 2:
        if (selector->mapMode) {
            MoveMapSelector(edit, selector);            
        } else {
            MoveObjectSelector(edit, selector);
        }
        break;

    case 3:
        if (StageMain(&stageState, editName) == MENU_TITLE_INIT) {
            stageState = 1;
        }
        break;
    }
    PrintWindow("fps: %d map size: (%d x %d) player pos: (%d, %d)",
                engineFps,
                edit->width, edit->height,
                selector->block_x, selector->block_y);
    return MENU_MAPEDIT;
}





