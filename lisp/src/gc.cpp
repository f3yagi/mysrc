#include <stdlib.h>
#include "lisp.h"
#define DEFALUT_GCOBJECT_SIZE 8000

struct GCObject {
    int next;
    enum LispType type;
    union {
        struct LispCons   cons;
        struct LispSymbol symbol;
        struct LispFlot   lfloat;
        struct LispString string;
   } u;
};
static struct GCObject *list = NULL;
static int nextPtr = -1, maxFreeList = 0;

static void MarkDelete(struct GCObject *o)
{
    switch (o->type) {
    case LISP_CONS:
        o->u.cons.gc = 0;
        break;

    case LISP_SYMBOL:
        o->u.symbol.gc = 0;
        break;

    case LISP_FLOAT:
        o->u.lfloat.gc = 0;
        break;

    case LISP_STRING:
        o->u.string.gc = 0;
        break;
    }
}

static int CheckFree(struct GCObject *o)
{
    switch (o->type) {
    case LISP_CONS:
        if (o->u.cons.gc == 0) return 1;
        break;

    case LISP_SYMBOL:
        if (o->u.symbol.gc == 0) {

            return 1;
        }
        break;

    case LISP_FLOAT:
        if (o->u.lfloat.gc == 0) return 1;
        break;

    case LISP_STRING:
        if (o->u.string.gc == 0) {
            if (o->u.string.str != NULL)
                free(o->u.string.str);
            return 1;
        }
        break;
    }
    return 0;
}    

static void GCSweep(void) 
{
    int i;
    
    for (i = 0; i < DEFALUT_GCOBJECT_SIZE; i++) {
        MarkDelete(&list[i]);
    }
}

void *GCAlloc(enum LispType type)
{
    struct GCObject *o;
    
    nextPtr++;
    if (nextPtr == maxFreeList) {
        LISP_ERROR("gc alloc error\n");
        exit(0);
    }
    o = &list[nextPtr];
    o->type = type;
    switch (o->type) {
    case LISP_CONS:
        return (void *)&o->u.cons;
        break;

    case LISP_SYMBOL:
        return (void *)&o->u.symbol;
        break;

    case LISP_FLOAT:
        return (void *)&o->u.lfloat;
        break;

    case LISP_STRING:
        return (void *)&o->u.string;
        break;
    }
    return NULL;
}

void InitGC(void)
{
    int i;
    
    list = (struct GCObject *)malloc(sizeof(struct GCObject) * DEFALUT_GCOBJECT_SIZE);
    for (i = 0; i < DEFALUT_GCOBJECT_SIZE; i++) {
        list[i].next = i - 1;
    }
    maxFreeList = DEFALUT_GCOBJECT_SIZE;
}

void ResetGC(void)
{
    nextPtr = -1;
}    

void QuitGC(void)
{
    //Lisp_printf("GCpointer: %d\n", nextPtr);
    if (list != NULL) free(list);
}

void PrintGC(void)
{
    Lisp_printf("GCpointer: %d\n", nextPtr);
}
