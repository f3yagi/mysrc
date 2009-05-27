extern int mapEdit; /* mapEdit flag */
extern int score;

struct ObjectData {
    int x, y; 
    char type;
};

struct MapData {
    char wall  : 1;  /* wall flag */
    char block : 7;  /*  block type */
};

typedef struct Map {
    float x, y;
    int width, height; /* FIELD width, height */
    int objectNum;
    struct MapData *data;
    struct ObjectData *object;
} Map;
/*-----------------------------------------------------*/
typedef struct Object {
    float x, y;
    int width, height; 
    float hit_x, hit_y;
    float hit_width, hit_height;
} Object;
#define TO_OBJECT(o) ((Object *)o)

#define X0(o) (o->x)
#define Y0(o) (o->y)
#define X1(o) (X0(o) + o->hit_width - 1)
#define Y1(o) (Y0(o) + o->hit_height - 1)
/* center x y */
#define CX(o) (X0(o) + (o->hit_width / 2.0f)  - 1)
#define CY(o) (Y0(o) + (o->hit_height / 2.0f) - 1)

enum Direction {
    DIR_RIGHT,
    DIR_LEFT,
    DIR_TOP
};

typedef struct Player {
    Object o;
    char jump;
    char die;
    char clear;
    float vy;
    enum Direction dir;
} Player;

typedef struct Enemy {
    Object o;    
    int life;    
    char die;
    char jump;
    float vy;
    void (*Move)(Map *map, Enemy *enemy);
    void (*Draw)(Map *map, Enemy *enemy);
    int data[10];
} Enemy;

typedef struct Item {
    Object o;
    char die;
    void (*Move)(Map *map, Item *item);
    void (*Draw)(Map *map, Item *item);
    void (*Effect)(Item *item);
} Item;

typedef struct Bomb {
    Object o;
    char die;
    float distance;
    enum Direction dir;
    void (*Move)(Map *map, Bomb *bomb);
    void (*Draw)(Map *map, Bomb *bomb);
} Bomb;

typedef struct Effect {
    Object o;
    char die;
    int frame;
    int state;
    void (*Move)(Map *map, Effect *effect);
    void (*Draw)(Map *map, Effect *effect);
} Effect;

enum EffectType {
    EFFECT_EFFECT1
};

/* action.cpp */
int DomainCheck(Map *map, Object *o);
int IsHit(Object *o1, Object *o2);
int IsLeftWall(Map *map, Object *o);
int IsRightWall(Map *map, Object *o);
int IsTopWall(Map *map, Object *o);
int IsBottomWall(Map *map, Object *o);
int IsTopWallJump(Map *map, Object *o);
int IsBottomWallJump(Map *map, Object *o);
void DrawBase(Map *map, int texture, Object *o);

/* enemy.cpp */
void InitEnemy();
void DrawEnemy(Map *map);
void MoveEnemy(Map *map);
int CreateEnemy(char type, float x, float y);
Enemy *EnemyIter(Map *map);
void InitEnemyIter();

/* item.cpp */
void InitItem();
void DrawItem(Map *map);
void MoveItem(Map *map);
int CreateItem(char type, float x, float y);

/* bomb.cpp */
void InitBomb();
void MoveBomb(Map *map);
void DrawBomb(Map *map);
void CreateBomb();

/* effect1.cpp */
void InitEffect();
void MoveEffect(Map *map);
void DrawEffect(Map *map);
int CreateEffect(enum EffectType type, float x, float y);

/* action_map.cpp */
#define BLOCK_WIDTH  32
#define BLOCK_HEIGHT 32
#define FIELD_X(v) (int)((v) / BLOCK_WIDTH) 
#define FIELD_Y(v) (int)((v) / BLOCK_HEIGHT)
Map *ReadMap(char *name);
void DrawMap(Map *map, int editMode);
void MoveMap(Map *map);
int IsWall(Map *map, int x, int y);
void CreateObject(Map *map);
void FreeMap(Map *map);
void InitMap(Map *map);

/* player.cpp */
void InitPlayer(float x, float y);
void MovePlayer(Map *map);
void DrawPlayer(Map *map);
int IsPlayerClear();
int IsPlayerDie();
Player *GetPlayer();

/*action_main.cpp*/
enum ActionMenu {
    MENU_RES,
    MENU_TITLE,
    MENU_TITLE_INIT,
    MENU_INIT,
    MENU_MAIN,
    MENU_MAPEDIT,
    MENU_STAGE,
    MENU_CLEAR,
    MENU_GAMEOVER
};
void ActionMain();
enum ActionMenu StageMain(int *stageState, char *mapName);

/* map_writer.cpp */
void QuitMapWriter();
enum ActionMenu MapWritterMain();
extern void (*SwitchObject)(int id, float x, float y);
extern void (*SwitchBlock)(int id, float x, float y);
void DrawLineHL(float y, float begin, float end, byte r, byte g, byte b);
void DrawLineVL(float x, float begin, float end, byte r, byte g, byte b);

/* res.cpp */
void InitResource();







