#ifdef __cplusplus
#define FARGS ...
#else
#define FARGS 
#endif
    
#define LVALUE long long /* 64bit */
//#define LVALUE int /* 32bit */

#define MANY_ARGS -1

enum LispType {
    LISP_CONS,
    LISP_SYMBOL,
    LISP_INTEGER,
    LISP_FLOAT,
    LISP_STRING,
    LISP_NIL,
    LISP_TRUE,
    LISP_FALSE
};

typedef struct LispObject {
    enum LispType type;
    LVALUE value;
} LispObject;

struct LispCons {
    char gc;
    LispObject car;
    LispObject cdr;
};

#define MAX_SYMBOL 256
    
struct LispSymbol {
    char gc;
    char *name;
};
    
struct LispCFunction {
    char name[256];
    char evalArg;
    int argc;
    LispObject (*func)(FARGS);    
};

struct LispUserFunction {
    char *name;
    char evalArg;
    LispObject body;
};

struct LispFlot {
    char gc;
    double value;
};

struct LispString {
    char gc;
    int len;
    char *str;
};
    
#define Lisp_printf Lisp_print
    
#define LISP_ERROR(format, ...) (Lisp_printf("%s %d:", __FILE__, __LINE__), \
                                 Lisp_printf(format, __VA_ARGS__),      \
                                 Lisp_exit()) 
#define TYPE(o) (o.type)

#define LOBJECT(o, type) ((type *)o.value)
#define LCONS(o)    LOBJECT(o, struct LispCons)  
#define LSYMBOL(o)  LOBJECT(o, struct LispSymbol)
#define LINTEGER(o) ((int)o.value) 
#define LFLOAT(o)   LOBJECT(o, struct LispFlot)
#define LSTRING(o)  LOBJECT(o, struct LispString)

#define SET_OBJECT(o, t, v) (o.type = t, o.value = (LVALUE)v)
#define SET_CONS(o, v)  SET_OBJECT(o, LISP_CONS, v)
#define SET_SYMBOL(o,v) SET_OBJECT(o, LISP_SYMBOL, v)
#define SET_INTEGER(o, v) (o.type = LISP_INTEGER, o.value = v) 
#define SET_FLOAT(o, v)  SET_OBJECT(o, LISP_FLOAT, v)
#define SET_STRING(o, v) SET_OBJECT(o, LISP_STRING, v)

#define CONS(o, c1, c2) (SET_CONS(o, CONS_NEW()),\
                         LCONS(o)->car = c1,     \
                         LCONS(o)->cdr = c2, o)

#define CAR(o) (CONSP(o)                        \
                ? LCONS(o)->car                 \
                : LISP_ERROR("car: type error\n"))

#define CDR(o) (CONSP(o)                        \
                ? LCONS(o)->cdr                 \
                : LISP_ERROR("cdr: type error\n"))

#define SET_CAR(o, v) (LCONS(o)->car = v)
#define SET_CDR(o, v) (LCONS(o)->cdr = v)

#define CONS_NEW()   ((struct LispCons *)GCAlloc(LISP_CONS))
#define SYMBOL_NEW() ((struct LispSymbol *)GCAlloc(LISP_SYMBOL))
#define FLOAT_NEW()  ((struct LispFlot *)GCAlloc(LISP_FLOAT)) 
#define STRING_NEW() ((struct LispString *)GCAlloc(LISP_STRING))

#define CHECK_OBJECT(o, t) (o.type == t                                 \
                            ? Qtrue                                     \
                            : LISP_ERROR("type error\n"))

#define CHECK_CONS(o)    CHECK_OBJECT(o, LISP_CONS)
#define CHECK_INTEGER(o) CHECK_OBJECT(o, LISP_INTEGER)
#define CHECK_SYMBOL(o)  CHECK_OBJECT(o, LISP_SYMBOL)
#define CHECK_FLOAT(o)   CHECK_OBJECT(o, LISP_FLOAT)
#define CHECK_STRING(o)  CHECK_OBJECT(o, LISP_STRING)

#define OBJECTP(o, t) (o.type == t)
#define CONSP(o)    OBJECTP(o, LISP_CONS)
#define INTEGERP(o) OBJECTP(o, LISP_INTEGER)
#define SYMBOLP(o)  OBJECTP(o, LISP_SYMBOL)
#define FLOATP(o)   OBJECTP(o, LISP_FLOAT)
#define STRINGP(o)  OBJECTP(o, LISP_STRING)
#define NILP(o)     OBJECTP(o, LISP_NIL)
#define TRUEP(o)    OBJECTP(o, LISP_TRUE)
    
extern LispObject Qnil, Qtrue;
    
#ifdef __cplusplus    
typedef LispObject (*CPLUS_FUNC)(FARGS);
#endif

#define DEF_SUBR(func, name, argc, evalArg)                     \
    DefineCFunction((CPLUS_FUNC)func, name, argc, evalArg)

/* gc.c */
void *GCAlloc(enum LispType type);
void InitGC(void);
void QuitGC(void);
void ResetGC(void);
void PrintGC(void);

/* lib.c */
int Lisp_print(char *format, ...);
LispObject Lisp_exit(void);
int Lisp_sprintf(char *buf, int size, char *fotmat, ...);
int Lisp_strcpy(char *buf1, int size, char *buf2);
void Lisp_InitLog(void);
int Lisp_WriteLog(char *format, ...);
void Lisp_QuitLog(void);

/* lisp.c */
void InitLisp(void);
LispObject PrintList(LispObject o);
LispObject Cons(LispObject car, LispObject cdr);
LispObject Car(LispObject o);
LispObject Cdr(LispObject o);
LispObject List(LispObject *o, int n);
int SerchLogicalSymbol(LispObject o, enum LispType type);

/* alloc.c */
void InitAlloc(void);
void QuitAlloc(void);
void *Malloc(int size);

LispObject MakeInteger(int value);
LispObject GC_MakeFloat(double value);
LispObject GC_MakeSymbol(char *name);
LispObject GC_MakeString(char *str);

LispObject Alloc_MakeFloat(double value);
LispObject Alloc_MakeSymbol(char *name);
LispObject Alloc_MakeString(char *str);

#define MakeFloat(value) GC_MakeFloat(value)
#define MakeSymbol(name) GC_MakeSymbol(name)
#define MakeString(str)  GC_MakeString(str)

LispObject CopyObject(LispObject o);

int SymbolEqual(LispObject symbol1, char *symbol2);
int StrEqual(char *str1, char *str2);

void DefineCFunction(LispObject (*func)(FARGS), char *name, int argc, char evalArg);
struct LispCFunction *GetCFunction(char *name);

void DefineUserFunction(LispObject symbol, char evalArg, LispObject body);
struct LispUserFunction *GetUserFunction(LispObject symbol);
void PrintUserFunction(void);

LispObject GetStack(LispObject symbol);
LispObject SearchSetStack(LispObject symbol, LispObject value);

void PushStack(LispObject symbol, LispObject value);
void PopStack(void);
void PrintStack(void);

void PushTemporaryStack(LispObject symbol, LispObject value);
void PopTemporaryStack(void);
void PrintTemporayStack(void);

/* eval.c */
LispObject Eval(LispObject o);
void InitEvel(void);

/* parse.c */
LispObject Cons2(LispObject car, LispObject cdr);
void LoadFromFile(char *name);

/* math.c */
void InitMath(void);

