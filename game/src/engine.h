#define ENGINE_WIDTH  640
#define ENGINE_HEIGTH 480

typedef unsigned char byte;
extern int engineFps;

/* main.cpp */
void WaitFps();
void QuitMainLoop();
void GetPreFrame();

/* video.cpp */
int InitVideo(int fullScreen, int width, int height);
int DrawEngine();
int ReadTexture(char *name);
int ReadSurface(char *name);
void DrawTexture(int id);
void DrawTextureClear();
void DrawSurface(int id, int x0, int y0, int x1, int y1);

void _DrawVertex(float x0, float y0, float x1, float y1,
                 float tu0, float tv0, float tu1, float tv1,
                 byte r, byte g, byte b, byte a);

#define DrawVertex(x0, y0, x1, y1, tu0, tv0, tu1, tv1, r, g, b)\
    _DrawVertex(x0, y0, x1, y1, tu0, tv0, tu1, tv1, r, g, b, 255)

int DrawEngine();
void DrawBackGround(byte r, byte g, byte b);

/* input.cpp */
int InitInput();
void ReadInput();

enum InputKey {
    KEY_UP,
    KEY_DOWN,
    KEY_LEFT,
    KEY_RIGHT,
    KEY_Z,
    KEY_X,
    KEY_C,
    KEY_A,
    KEY_S,
    KEY_D,
    KEY_F
};
int PushKey(enum InputKey key);

extern void (*GameMain)();
extern void (*DrawMain)();

/* sound.cpp */
int InitSound(int on);
extern int (*LoadSound)(char *name);
void LoopSound(int id);
void SoundPlay(int id);
void FreeSound(int id);
void StopSound();
void EndSound();

/* pack.cpp */
void InitPack(char *name);
char *LoadFile(char *name, int *size);
/*---------*/

struct Win32Input {
    void (*KeyZ)();
    void (*KeyX)();
    void (*KeyC)();
    void (*KeyA)();
    void (*KeyS)();
    void (*KeyD)();
    void (*KeyW)();
    void (*KeyQ)();
    void (*KeyF)();
    void (*KeyE)();
    void (*KeyR)();
    void (*KeyT)();
};
extern struct Win32Input winput;

/* action_main.cpp */
void OptionParse(int argc, char **argv);

/* texture_helper.cpp */
void ReadTextureObject(char *name);
int GetTextrueObject(char *name);
void DrawTextureObject(int id, float x, float y);

/* font.cpp */
void InitFont();
void FontPrintf(float x, float y, char *format, ...);

/* res.cpp */
void InitResource();

















