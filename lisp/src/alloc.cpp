#include <stdlib.h>
#include <string.h>
#include "lisp.h"

struct MemAllocator {
    int size;
    void *mem;
};
static struct MemAllocator *mallocator = NULL;
static int mallocatorSize;
static int mallocatorPointer;
#define DEFAULT_MALLOCATOR_SIZE 4000

static void InitMallocator(void)
{
    mallocator = (struct MemAllocator *)calloc(DEFAULT_MALLOCATOR_SIZE,
                                               sizeof(struct MemAllocator));
    mallocatorSize = DEFAULT_MALLOCATOR_SIZE;
    mallocatorPointer = 0;    
}

static void QuitMallocator(void)
{
    int i;
    
    for (i = 0; i < mallocatorSize; i++) {
        if (mallocator[i].mem != NULL) {
            free(mallocator[i].mem);
        }
    }
    if (mallocator[i].mem != NULL) {
        free(mallocator);
    }
}

void *Malloc(int size)
{
    int n = mallocatorPointer;
    
    if (mallocatorPointer == mallocatorSize) {
        mallocator = (struct MemAllocator *)realloc(mallocator,
                                                    sizeof(struct MemAllocator) *
                                                    mallocatorSize * 2);
        mallocatorSize *= 2;
        if (mallocator == NULL) LISP_ERROR("alloc error\n");
    }
    mallocatorPointer++;
    mallocator[n].mem  = malloc(size);
    mallocator[n].size = size;
    //Lisp_printf("%s: %d: mallocator %d\n", __FILE__, __LINE__, mallocatorPointer);
    return mallocator[n].mem;
}

static char *StringAlloc(char *name)
{
    int n = (int)strlen(name);
    char *r = (char *)Malloc(n + 1);
    memcpy(r, name, n);
    r[n] = '\0';
    return r;
}    

LispObject MakeInteger(int value)
{
    LispObject o;
    
    SET_INTEGER(o, value);
    return o;
}

LispObject GC_MakeFloat(double value)
{
    LispObject o;
    struct LispFlot *f = FLOAT_NEW();

    f->value = value;
    SET_FLOAT(o, f);
    return o;
}

LispObject Alloc_MakeFloat(double value)
{
    LispObject o;
    struct LispFlot *f = (struct LispFlot *)Malloc(sizeof(struct LispFlot));

    f->value = value;
    SET_FLOAT(o, f);
    return o;
}

LispObject GC_MakeSymbol(char *name)
{
    LispObject o;
    struct LispSymbol *s = SYMBOL_NEW();
    
    s->name  = StringAlloc(name);
    SET_SYMBOL(o, s);
    return o;
}

LispObject Alloc_MakeSymbol(char *name)
{
    LispObject o;
    struct LispSymbol *s = (struct LispSymbol *)Malloc(sizeof(struct LispSymbol));
    
    s->name  = StringAlloc(name);
    SET_SYMBOL(o, s);
    return o;
}

LispObject GC_MakeString(char *str)
{
    LispObject o;
    struct LispString *s = STRING_NEW();
    
    s->str = StringAlloc(str);
    s->len = (int)strlen(str);
    SET_STRING(o, s);
    return o;
}

LispObject Alloc_MakeString(char *str)
{
    LispObject o;
    struct LispString *s = (struct LispString *)Malloc(sizeof(struct LispString));
    
    s->str = StringAlloc(str);
    s->len = (int)strlen(str);
    SET_STRING(o, s);
    return o;
}

LispObject CopyObject(LispObject o)
{
    LispObject r = Qnil;
    
    switch (TYPE(o)) {
    case LISP_INTEGER:
        r = o;
        break;
        
    case LISP_SYMBOL:
        r = Alloc_MakeSymbol(LSYMBOL(o)->name);
        break;

    case LISP_FLOAT:
        r = Alloc_MakeFloat(LFLOAT(o)->value);
        break;
        
    case LISP_STRING:
        r = Alloc_MakeString(LSTRING(o)->str);
        break;
    }
    return r;
}    

int StrEqual(char *str1, char *str2)
{
    int r;
    r = strcmp(str1, str2);
    return !r;
}    

static int StringEqual(LispObject str1, LispObject str2)
{
    int r;
    
    CHECK_STRING(str1);
    CHECK_STRING(str2);
    r = StrEqual(LSTRING(str1)->str, LSTRING(str2)->str);
    return r;
}

int SymbolEqual(LispObject symbol1, char *symbol2)
{
    int r;

    CHECK_SYMBOL(symbol1);
    r = StrEqual(LSYMBOL(symbol1)->name, symbol2);
    return r;
}    

#define MAX_CFUNCTION 1000
static struct LispCFunction Cfunction[MAX_CFUNCTION];
static int CfunctionPointer = 0;

void DefineCFunction(LispObject (*func)(FARGS), char *name, int argc, char evalArg)
{
    int i = CfunctionPointer;
    if (CfunctionPointer == MAX_CFUNCTION) {
        LISP_ERROR("max error\n");
        return;
    }
    strcpy_s(Cfunction[i].name, 256, name);
    Cfunction[i].evalArg = evalArg;
    Cfunction[i].argc    = argc;
    Cfunction[i].func    = func;
    CfunctionPointer++;
}

struct LispCFunction *GetCFunction(char *name)
{
    int i;
    
    for (i = 0; i < CfunctionPointer; i++) {
        int r = StrEqual(Cfunction[i].name, name);
        if (r) return &Cfunction[i];
    }
    return NULL;
}

void CheckDefineFunction(void)
{
    int i;
    
    for (i = 0; i < CfunctionPointer; i++) {
        Lisp_printf("%d [%s]\n", i, Cfunction[i].name);
    }
}
#define MAX_STACK_SIZE 300

struct StackFrame {
    char *name;
    LispObject value;
};

struct LispStack {
    int stackPointer;
    struct StackFrame stack[MAX_STACK_SIZE];
};
static struct LispStack tmp, stack;

static void LispPrintStack(struct LispStack *s, char *name)
{
    int i;
    struct StackFrame *stack = s->stack;
    
    Lisp_printf("%s: stack pointer %d\n", name, s->stackPointer);
    for (i = 0; i < s->stackPointer; i++) {
        Lisp_printf("stack[%d] [%s] ", i, stack[i].name);
        PrintList(stack[i].value);
    }
}

void PrintStack(void)
{
    LispPrintStack(&stack, "stack");
}

void PrintTemporayStack(void)
{
    LispPrintStack(&tmp, "temporary stack");
}

static LispObject LispGetStack(struct LispStack *s, LispObject symbol)
{
    int i;
    struct StackFrame *stack = s->stack;
    
    CHECK_SYMBOL(symbol);
    for (i = s->stackPointer - 1; i >= 0; i--) {
        int r = SymbolEqual(symbol, stack[i].name);
        if (r) {
            return stack[i].value;
        }
    }
    return Qnil;
}

LispObject GetStack(LispObject symbol)
{
    LispObject r;
    
    r = LispGetStack(&tmp, symbol);
    if (!NILP(r)) return r;
    return LispGetStack(&stack, symbol);
}

static LispObject SelfValue(LispObject o1, LispObject o2)
{
    if (TYPE(o1) == TYPE(o2)) {
        switch (TYPE(o2)) {
        case LISP_INTEGER:
            o1 = o2;
            break;
            
        case LISP_SYMBOL:
            LSYMBOL(o1)->name = StringAlloc(LSYMBOL(o2)->name);
            break;

        case LISP_STRING:
            LSTRING(o1)->str = StringAlloc(LSTRING(o2)->str);
            break;
            
        case LISP_FLOAT:
            LFLOAT(o1)->value = LFLOAT(o2)->value;
            break;
        }
    } else {
        o1 = CopyObject(o2);
    }
    return o1;
}    

static LispObject LispSetStack(struct LispStack *s, LispObject symbol, LispObject value)
{
    int i;
    struct StackFrame *stack = s->stack;    
    
    CHECK_SYMBOL(symbol);
    for (i = s->stackPointer - 1; i >= 0; i--) {
        int r = SymbolEqual(symbol, stack[i].name);
        if (r) {
            stack[i].value = SelfValue(stack[i].value, value);
            return Qtrue;
        }
    }
    return Qnil;
}

LispObject SearchSetStack(LispObject symbol, LispObject value)
{
    return LispSetStack(&stack, symbol, value);
}

static void LispPushStack(struct LispStack *s, LispObject symbol, LispObject value)
{
    struct StackFrame *stack = s->stack;
    
    CHECK_SYMBOL(symbol);
    stack[s->stackPointer].name  = StringAlloc(LSYMBOL(symbol)->name);
    stack[s->stackPointer].value = value;
    s->stackPointer++;
    if (s->stackPointer == MAX_STACK_SIZE) {
        LISP_ERROR("stack over\n");
    }
}

void PushStack(LispObject symbol, LispObject value)
{
    LispPushStack(&stack, symbol, value);
}

void PushTemporaryStack(LispObject symbol, LispObject value)
{
    LispPushStack(&tmp, symbol, value);
}

static void LispPopStack(struct LispStack *s)
{
    if (0 <= s->stackPointer - 1) s->stackPointer--;
}

void PopStack(void)
{
    LispPopStack(&stack);
}

void PopTemporaryStack(void)
{
    LispPopStack(&tmp);
}

static void InitStack(void)
{
    tmp.stackPointer = 0;
    stack.stackPointer = 0;
}

void InitAlloc(void)
{
    InitMallocator();
    InitStack();
}

void ResetAlloc(void)
{
    
}    

void QuitAlloc(void)
{
    QuitMallocator();
}
