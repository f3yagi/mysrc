#include "engine.h"
#include "action.h"
#include "res.h"

#define MAX_ITEM 256
static Item item[MAX_ITEM];
static int itemSize;

void InitItem()
{
    itemSize = 0;
}

void MoveItem(Map *map)
{
    int i;
    
    for (i = 0; i < itemSize; i++) {
        if (!item[i].die &&
            !DomainCheck(map, TO_OBJECT(&item[i]))) {
            item[i].Move(map, &item[i]);
        }
    }
}

void DrawItem(Map *map)
{
    int i;

    for (i = 0; i < itemSize; i++) {
        if (!item[i].die &&
            !DomainCheck(map, TO_OBJECT(&item[i]))) {
            item[i].Draw(map, &item[i]);
        }
    }
}    

static void MoveItemBase(Map *map, Item *item)
{
    Player *player = GetPlayer();
    Object *o = TO_OBJECT(item);
    
    if (IsHit(TO_OBJECT(player), o)) {
        item->Effect(item);
        item->die = 1;
    }
}

static void AppleEffect(Item *item)
{
    score += 10;
}

static void DrawApple(Map *map, Item *item)
{
    DrawBase(map, res.item_apple, TO_OBJECT(item));
}    

static void ItemApple(Item *item , float x, float y)
{
    TO_OBJECT(item)->x      = x;
    TO_OBJECT(item)->y      = y;
    TO_OBJECT(item)->width  = 32;
    TO_OBJECT(item)->height = 32;

    TO_OBJECT(item)->hit_x      = 0;
    TO_OBJECT(item)->hit_y      = 0;
    TO_OBJECT(item)->hit_width  = 32;
    TO_OBJECT(item)->hit_height = 32;

    item->die = 0;
    item->Move = MoveItemBase;
    item->Draw = DrawApple;
    item->Effect = AppleEffect;
}    

int CreateItem(char type, float x, float y)
{
    switch (type) {
    case ITEM_APPLE:
        ItemApple(&item[itemSize], x, y);
        goto end;
        break;
    }
    return 0;
 end:
    itemSize++;
    return 1;
}    
