#include "engine.h"
#include "res.h"
#include "action.h"

/* global */
struct Resource res;
void (*SwitchObject)(int id, float x, float y);
void (*SwitchBlock)(int id, float x, float y);

static void DrawBlock(int id, float x, float y)
{
    switch (id) {
    case BLOCK_BLOCK1:
        DrawTextureObject(res.block_block1, x, y);
        break;
        
    case BLOCK_DOOR:
        DrawTextureObject(res.block_door, x, y);
        break;
    }
}

static void DrawObject(int id, float x, float y)
{
    switch (id) {
    case PLAYER:
        DrawTextureObject(res.player_right, x, y);
        break;

    case ENEMY_BIRD:
        DrawTextureObject(res.enemy_bird, x, y);        
        break;

    case ENEMY_DOG:
        DrawTextureObject(res.enemy_dog, x, y);                
        break;

    case ITEM_APPLE:
        DrawTextureObject(res.item_apple, x, y);                
        break;
    }
}

void InitResource()
{
    InitPack("data.dat");
    /*---------------------------------------------*/
    ReadTextureObject("img\\font.txt");
    res.font_stage = GetTextrueObject("font_stage");
    res.font_score = GetTextrueObject("font_score");
    /*---------------------------------------------*/
    ReadTextureObject("img\\chare.txt");
    res.player_right = GetTextrueObject("player_right");
    res.player_left  = GetTextrueObject("player_left");
    res.enemy_bird   = GetTextrueObject("bird");
    res.block_block1 = GetTextrueObject("map_green");
    res.item_apple   = GetTextrueObject("apple");
    res.enemy_dog    = GetTextrueObject("dog");
    /*---------------------------------------------*/
    ReadTextureObject("img\\effect1.txt");
    res.effect1_0 = GetTextrueObject("effect1_1");
    res.effect1_1 = GetTextrueObject("effect1_2");
    res.effect1_2 = GetTextrueObject("effect1_3");
    res.effect1_3 = GetTextrueObject("effect1_4");
    res.effect1_4 = GetTextrueObject("effect1_5");
    res.effect1_5 = GetTextrueObject("effect1_6");
    res.effect1_6 = GetTextrueObject("effect1_7");
    res.effect1_7 = GetTextrueObject("effect1_8");
    res.effect1_8 = GetTextrueObject("effect1_9");
    res.effect1_9 = GetTextrueObject("effect1_10");
    /*---------------------------------------------*/
    res.sound_jump  = LoadSound("sound\\jump.wav");
    res.sound_die   = LoadSound("sound\\die.wav");
    InitFont();
    /*---------------------------------------------*/
    SwitchObject = DrawObject;
    SwitchBlock  = DrawBlock;
}    
