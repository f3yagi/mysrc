#include "engine.h"
#include "action.h"
#include "res.h"

#define MAX_EFFECT 256
static Effect effect[MAX_EFFECT];
static int effectSize = 0;

void InitEffect()
{
    effectSize = 0;
}

void MoveEffect(Map *map)
{
    int i;

    for (i = 0; i < effectSize; i++) {
        if (!effect[i].die) {
            effect[i].Move(map, &effect[i]);
        }
    }
}

void DrawEffect(Map *map)
{
    int i;

    for (i = 0; i < effectSize; i++) {
        if (!effect[i].die) {
            effect[i].Draw(map, &effect[i]);
        }
    }
}

static void MoveEffect1(Map *map, Effect *effect)
{
    
}    

static void DrawEffect1(Map *map, Effect *effect)
{
#define DRAW(n) case n: DrawBase(map, res.effect1_##n, TO_OBJECT(effect)); break
    switch (effect->state) {
        DRAW(0);
        DRAW(1);
        DRAW(2);
        DRAW(3);
        DRAW(4);
        DRAW(5);
        DRAW(6);
        DRAW(7);
        DRAW(8);
        DRAW(9);
    default:
        effect->die = 1;
        break;
    }
    if (effect->frame == 5) {
        effect->frame = 0;
        effect->state++;
    }
    effect->frame++;
#undef DRAW    
}    

static void EffectEffect1(Effect *effect, float x, float y)
{
    TO_OBJECT(effect)->x = x;
    TO_OBJECT(effect)->y = y;
    TO_OBJECT(effect)->width  = 32;
    TO_OBJECT(effect)->height = 32;

    TO_OBJECT(effect)->hit_x = 0;
    TO_OBJECT(effect)->hit_y = 0;
    TO_OBJECT(effect)->hit_width  = 32;
    TO_OBJECT(effect)->hit_height = 32;

    effect->state = 0;
    effect->frame = 0;
    effect->Move = MoveEffect1;
    effect->Draw = DrawEffect1;
    effect->die  = 0;
}    

int CreateEffect(enum EffectType type, float x, float y)
{
    switch (type) {
    case EFFECT_EFFECT1:
        EffectEffect1(&effect[effectSize], x, y);
        goto end;
        break;
    }
    return 0;
 end:
    effectSize++;
    return 1;
}    
