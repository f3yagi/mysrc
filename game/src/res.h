enum BlockType {
    BLOCK_NODEF,
    BLOCK_BLOCK1,
    BLOCK_DOOR,
    BLOCK_END
};

enum ObjectType {
    OBJECT_NODEF, /* start point */
    PLAYER,
    ENEMY_BIRD,
    ENEMY_DOG,
    ITEM_APPLE,
    OBJECT_END, /* end point */
};

struct Resource {
    int player_left;
    int player_right;
    int enemy_bird;
    int block_door;
    int block_block1;
    int item_apple;
    int enemy_dog;
    /*------------*/
    int font_stage;
    int font_score;
    /*------------*/        
    int title;
    /*------------*/            
    int block_bla;
    int block_blue;
    int block_green;
    /*------------*/        
    int sound_jump;
    int sound_die;
    /*------------*/
    int effect1_0;
    int effect1_1;
    int effect1_2;
    int effect1_3;
    int effect1_4;
    int effect1_5;
    int effect1_6;
    int effect1_7;
    int effect1_8;
    int effect1_9;
};
extern struct Resource res;
