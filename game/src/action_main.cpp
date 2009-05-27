#include <stdio.h>
#include <stdlib.h>
#include "engine.h"
#include "action.h"
#include "lib.h"
#include "res.h"
#include "conf.h"

/* global */
int score = 0;

void OptionParse(int argc, char **argv)
{
    if (argc <= 1) {
        return;
    }
}    

void TitleDraw()
{
    DrawSurface(res.title, 0, 0, 640, 480);
}

static Map *map = NULL;
static int stageNum = 0;
static int life;

void StageDraw()
{
    DrawMap(map, 0);
    DrawEnemy(map);
    DrawItem(map);
    DrawBomb(map);
    DrawEffect(map);
    DrawPlayer(map);
    
#if !MAP_EDIT_MODE
    DrawTextureObject(res.player_right, 0, 0);
    FontPrintf(32, 16, "x %d", life);
    
    DrawTextureObject(res.font_stage, 128, 16);
    FontPrintf(128 + 16 * 5, 16, "%d", stageNum + 1);

    DrawTextureObject(res.font_score, 128 + 16 * 5 + 32, 16);
    FontPrintf(128 + 16 * 5 + 32 + 16 * 5 + 16 , 16, "%d", score);
    PrintWindow("fps: %d", engineFps);
#endif    
}

static enum ActionMenu menuState = MENU_RES;
static int stageState = 0;

void ActionMain()
{
    switch (menuState) {
    case MENU_RES:
#if MAP_EDIT_MODE
        menuState = MENU_MAPEDIT;
#else        
        menuState = MENU_TITLE;
#endif        
        break;

    case MENU_MAPEDIT:
        menuState = MapWritterMain();
        break;

    case MENU_TITLE_INIT:
        //DrawMain = TitleDraw;
        menuState = MENU_TITLE;
        break;
        
    case MENU_TITLE:
        //if (PushKey(KEY_Z)) {
            score = 0;
            life = 2;
            stageNum = 0;
            menuState = MENU_STAGE;
            //}
        break;
        
    case MENU_STAGE:
        switch (stageNum) {
        case 0:
            menuState = StageMain(&stageState, "map\\stage1.map");
            break;

        case 1:
            menuState = StageMain(&stageState, "map\\stage2.map");
            break;

        case 2:
            menuState = StageMain(&stageState, "map\\stage3.map");
            break;
        }
        break;

    case MENU_GAMEOVER:
        menuState = MENU_TITLE_INIT;
        break;

    case MENU_CLEAR:
        if (stageNum == 2) {
            menuState = MENU_TITLE_INIT;
        } else {
            stageNum++;
            menuState = MENU_STAGE;            
        }
        break;
    }
}

void FreeStage(Map *map, int *stageState, int bgm)
{
    EndSound();
    DrawMain = NULL;
    *stageState = 0;
    FreeMap(map);
    FreeSound(bgm);
}    

void GetBgmName(char *bgmName, char *name)
{
    int i = 0, j = 0, n;
    char tmp[MAX_PATH];
    char *buf;

    RemoveExt(tmp, name);
    StrCat(tmp, MAX_PATH, ".txt");
    buf = LoadFile(tmp, &n);
    while (buf[j] != '\0') {
        if (buf[j] == ';') {
            break;
        }
        tmp[i] = buf[j];
        tmp[i + 1] = '\0';
        i++;
        j++;
    }
    StrCpy(bgmName, MAX_PATH, tmp);
    free(buf);
}

enum ActionMenu StageMain(int *stageState, char *mapName)
{
    static int bgm = - 1;
    static char name[MAX_PATH];
    
    switch (*stageState) {
    case 0:
        map = ReadMap(mapName);
#if !MAP_EDIT_MODE
        GetBgmName(name, mapName);
        bgm = LoadSound(name);
#endif        
        *stageState = 1;
        break;

    case 1:
        {
            InitMap(map);
            CreateObject(map);
            DrawMain = StageDraw;
            LoopSound(bgm);
            *stageState = 2;
        }
        break;

    case 2:
            MoveMap(map);
            MoveEnemy(map);
            MoveItem(map);
            MoveBomb(map);
            MoveEnemy(map);
            MovePlayer(map);
            if (IsPlayerDie()) {
                SoundPlay(res.sound_die);
                life--;
                if (life < 0) {
                    FreeStage(map, stageState, bgm);
                    return MENU_GAMEOVER;
                }
                *stageState = 1;
            }
            if (IsPlayerClear()) {
                FreeStage(map, stageState, bgm);
                return MENU_CLEAR;
            }
            break;
    }
    return MENU_STAGE;
}
