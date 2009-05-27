#include "engine.h"
#include "lib.h"

struct TextureObject {
    char name[MAX_PATH];
    int texture;
    int width, height;
    float tu0, tv0, tu1, tv1;
};
#define MAX_OBJECT 256
static struct TextureObject object[MAX_OBJECT];
static int objectSize = 0;

static void SetVal(struct TextureObject *object, char *tmp, int n)
{
    switch (n) {
    case 0:
        StrCpy(object->name, MAX_PATH, tmp);
        break;

    case 1:
        object->width = Atoi(tmp);
        break;

    case 2:
        object->height = Atoi(tmp);
        break;

    case 3:
        object->tu0 = (float)Atof(tmp);
        break;

    case 4:
        object->tv0 = (float)Atof(tmp);
        break;
        
    case 5:
        object->tu1 = (float)Atof(tmp);
        break;

    case 6:
        object->tv1 = (float)Atof(tmp);
        break;
    }
}

static void ParseLine(struct TextureObject *object, char *buf)
{
    int i = 0, n = 0;
    char tmp[MAX_PATH];
    
    while (*buf != '\0') {
        if (*buf == ' ') {
            SetVal(object, tmp, n);
            i = 0;
            n++;
        }
        tmp[i] = *buf;
        tmp[i + 1] = '\0';
        i++;
        buf++;
    }
    SetVal(object, tmp, n);
}

void ReadTextureObject(char *name)
{
    int i = 0, j = 0, texture, n;
    char tmp[MAX_PATH];
    char *buf;

    buf = LoadFile(name, &n);
    RemoveExt(tmp, name);
    StrCat(tmp, MAX_PATH, ".dds");
    texture = ReadTexture(tmp);
    while (buf[j] != '\0') {
        if (buf[j] == '\n') {
            object[objectSize].texture = texture;
            ParseLine(&object[objectSize], tmp);
            objectSize++;
            i = 0;
            j++;
        }
        tmp[i] = buf[j];
        tmp[i + 1] = '\0';
        i++;
        j++;
    }
    object[objectSize].texture = texture;
    ParseLine(&object[objectSize], tmp);
    objectSize++;
    Free(buf);
}

int GetTextrueObject(char *name)
{
    int i;

    for (i = 0; i < objectSize; i++) {
        if (!StrCmp(name, object[i].name)) {
            return i;
        }
    }
    EngineError("not find %s\n", name);
    return -1;
}    

void DrawTextureObject(int id, float x, float y)
{
    struct TextureObject *tobject;
    
    tobject = &object[id];
    DrawTexture(tobject->texture);
    DrawVertex(x, y, x + tobject->width, y + tobject->height,
               tobject->tu0, tobject->tv0, tobject->tu1, tobject->tv1,
               255, 255, 255);
}    
