#include <stdio.h>
#include "lib.h"

/* lib.cpp */
FILE *FOpen(char *name, char *mode);
void FClose(FILE *fp);

struct Data {
    char name[MAX_PATH];
    int  size;
    char *data;
    int ptr;
};

typedef struct PackFile {
    int num;
    struct Data *file;
} PackFile;

void EncodeBuf(char *buf, int size, int t)
{
    int i, j = 0, l = 32212;
    int k = 12876;
    
    for (i = 0; i < size; i++) {
        j++;
        l += 8831;
        if (j < size / 2) {
            k = k * 13323 + 211 - l;
        } else {
            k = k - 322 * 4 + l;
        }
        if (t) {
            buf[i] += k;
        } else {
            buf[i] -= k;
        }
    }
}

void EncodeName(char *name, int t)
{
    int i, j = 0, l = 821;
    int k = 31333;
    
    for (i = 0; i < MAX_PATH; i++) {
        j++;
        l += 31188;
        if (i > MAX_PATH / 3) {
            k = k * 32217 - 324;
        } else {
            k = j * 2112 - k + l;
        }
        if (t) {
            name[i] += k;
        } else {
            name[i] -= k;
        }
    }
}

static PackFile *PackOpenFromFile(char *name)
{
    FILE *fp;
    PackFile *pack;
    int num, i;
    int packPtr = 0;
    
    fp = FOpen(name, "rb");
    if (fp == NULL) {
        EngineError("not find %s\n", name);
    }
    pack = (PackFile *)Malloc(sizeof(PackFile));
    fread(&num, sizeof(int), 1, fp);
    packPtr += sizeof(int);
    
    pack->file = (struct Data *)Malloc(sizeof(struct Data) * num);
    pack->num  = num;
    for (i = 0; i < pack->num; i++) {
        struct Data *file = &pack->file[i];
        fread(file->name, sizeof(char), MAX_PATH, fp);
        EncodeName(file->name, 0);
        packPtr += sizeof(char) * MAX_PATH;
        fread(&file->size, sizeof(int), 1, fp);
        packPtr += sizeof(int);
        file->ptr = packPtr;
        fseek(fp, file->ptr + file->size, SEEK_SET);
        packPtr += file->size;
        file->data = NULL;
    }
    FClose(fp);
    return pack;
}

static FILE *packFile = NULL;

static PackFile *PackOpen(char *name)
{
    FILE *fp;
    char tmp[MAX_PATH];

    RemoveExt(tmp, name);
    StrCat(tmp, MAX_PATH, ".txt");
    fp = FOpen(tmp, "r");
    if (fp) {
        fgets(tmp, MAX_PATH, fp);
        FClose(fp);
        packFile = FOpen(tmp, "rb");
        return PackOpenFromFile(tmp);        
    }
    packFile = FOpen(name, "rb");
    return PackOpenFromFile(name);
}    

static void PackClose(PackFile *pack)
{
    if (!pack) {
        WriteLog("free pack file\n");        
        return;
    }
    Free(pack->file);
    Free(pack);
    if (packFile != NULL) {
        FClose(packFile);
    }
    WriteLog("free pack file\n");
}

static char *PackRead(PackFile *pack, char *name, int *size, FILE *fp)
{
    int i;

    if (!pack) {
        EngineError("not open pack file\n");
    }
    for (i = 0; i < pack->num; i++) {
        struct Data *file = &pack->file[i];
        if (!StrCmp(file->name, name)) {
            char *data;
            data  = (char *)Malloc(sizeof(char) * file->size);
            fseek(fp, file->ptr, SEEK_SET);
            fread(data, sizeof(char),  file->size, fp);
            EncodeBuf(data, file->size, 0);
            *size = file->size;
            WriteLog("read from pack file %s\n", name);
            return data;
        }
    }
    EngineError("not find %s\n", name);
    return NULL;
}

static PackFile *pack = NULL;

static void QuitPack();

void InitPack(char *name)
{
    AddFinalize(QuitPack);
    pack = PackOpen(name);
}

char *ReadFromPack(char *name, int *size)
{
    return PackRead(pack, name, size, packFile);
}

static void QuitPack()
{
    PackClose(pack);
}    

char *ReadFileBuf(char *name, int *size)
{
    FILE *fp;
    char *buf, *ext;
    int txt = 1;
    int n;

    ext = GetExt(name);
    if (!StrCmp(ext, ".txt")) {
        txt = 1;
    } else {
        txt = 0;
    }
    fp = FOpen(name, "rb");
    if (fp == NULL) {
        return NULL;
    }
    fseek(fp, 0, SEEK_END);
    n = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    buf = (char *)Malloc(sizeof(char) * n);
    fread(buf, sizeof(char), n, fp);
    if (txt) {
        buf[n - 1] = '\0';
    }
    FClose(fp);
    *size = n;
    return buf;
}

char *LoadFile(char *name, int *size)
{
    char *buf;
    
    buf = ReadFileBuf(name, size);
    if (buf != NULL) {
        WriteLog("load from file: %s\n", name);
        goto next;
    }
    buf = ReadFromPack(name, size);
    if (buf != NULL) {
        goto next;
    }
    EngineError("not find: %s\n", name);
next:
    return buf;
}    









