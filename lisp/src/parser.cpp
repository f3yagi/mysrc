#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "lisp.h"

#undef MakeFloat
#undef MakeSymbol
#undef MakeString
#define MakeFloat(value) Alloc_MakeFloat(value)
#define MakeSymbol(name) Alloc_MakeSymbol(name)
#define MakeString(str)  Alloc_MakeString(str)

#undef CONS_NEW
#define CONS_NEW() (struct LispCons *)Malloc(sizeof(struct LispCons))

LispObject Cons2(LispObject car, LispObject cdr)
{
    LispObject o;
    CONS(o, car, cdr);
    return o;
}

#define SYNTAX_ERROR(format, ...)                               \
    do {                                                        \
        syntax.parseError = 1;                                  \
        Lisp_printf("syntax error: line:%d ", syntax.line);     \
        Lisp_printf(format, __VA_ARGS__);                       \
    } while (0)

#define MAX_BUF_SIZE 256

static char *buf = NULL;
LispObject Parse(void);

struct Syntax {
    int begin;
    int end;
    int parseError;
    int line;
};
static struct Syntax syntax;

/* lib.c */
FILE *Lisp_fopen(char *name, char *mode);
void Lisp_fclose(FILE *fp);

static char *ReadFile(char *name)
{
    FILE *fp; 
    size_t size;
    char *buf;
    
    fp = Lisp_fopen(name, "r");
    if (fp == NULL) {
        return NULL;
    }
    fseek(fp, 0, SEEK_END);
    size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    buf = (char *)calloc(size + 1, 1);
    fread(buf, 1, size, fp);
    Lisp_fclose(fp);
    return buf;
}

static char *ReadToken(char *tmp)
{
    int n = 0;
    while (*buf != '\0') {
        if (*buf == ' ' || *buf == ')') {
            return buf;
        } 
        if (!isspace(*buf)) {
            tmp[n] = *buf;
            tmp[n + 1] = '\0';
            n++;
        }
        buf++;  
    }
    return buf;
}    

static LispObject ReadSymbol(void)
{
    char tmp[MAX_BUF_SIZE];
    ReadToken(tmp);
    if (StrEqual(tmp, "t")) {
        return Qtrue;
    }
    if (StrEqual(tmp, "nil")) {
        return Qnil;
    }
    return MakeSymbol(tmp);
}

static LispObject ReadNumber(void)
{
    int i = 0;
    char flotFlag = 0;
    char tmp[MAX_BUF_SIZE];
    
    ReadToken(tmp);
    while (tmp[i] != '\0') {
        if (tmp[i] == '.') flotFlag = 1;
        i++;
    }
    if (flotFlag) {
        return MakeFloat(atof(tmp));
    }
    return MakeInteger(atoi(tmp));
}

static LispObject ReadMinusNumber(void)
{
    if (isdigit(*buf)) {
        return ReadNumber();
    }
    return MakeSymbol("-");
}

static LispObject ReadString(void)
{
    char tmp[MAX_BUF_SIZE];
    int n = 0;
    
    while (*buf != '\0') {
        if (*buf == '"') {
            buf++;
            break;
        }
        tmp[n] = *buf;
        tmp[n + 1] = '\0';
        n++;
        buf++;
    }
    return MakeString(tmp);    
}

static char *SpaceSkip(char *buf)
{
    while (*buf != '\0') {
        if (isspace(*buf) == 0) {
            return buf;
        }
        buf++;
    }
    return buf;
}

static char *CommentSkip(char *buf)
{
    while (*buf != '\0') {
        if (*buf == '\n') return buf;
        buf++;
    }
    return buf;
}    

static LispObject ReadList(void)
{
    LispObject tmp =  Cons2(Qnil, Qnil);
    LispObject r   = tmp;
    int i = 0;
    
    buf = SpaceSkip(buf);
    if (*buf == '\0') return Qnil;
    if (*buf == ')') {
        buf++;
        return Qnil;
    }
    while (*buf != '\0') {
        SET_CAR(tmp, Parse());
        buf = SpaceSkip(buf);
        if (*buf == ')') {
            buf++;
            return r;
        }
        if (*buf == '.') {
            buf++;
            SET_CDR(tmp, Parse());
            return r;
        }
        SET_CDR(tmp, Cons2(Qnil, Qnil));
        tmp = CDR(tmp);
    }
    return r;
}

static LispObject ReadQuote(void)
{
    LispObject tmp = Cons2(Qnil, Qnil);
    LispObject r = tmp;

    SET_CAR(tmp, MakeSymbol("quote"));
    SET_CDR(tmp, Cons2(Qnil, Qnil));
    tmp = CDR(tmp);
    SET_CAR(tmp, Parse());
    return r;
}

static LispObject Parse(void)
{
    while (*buf != '\0') {
        switch (*buf) {
        case ';': /* comment */
            buf = CommentSkip(buf);
            break;
            
        case '(': /* list */
            buf++;
            return ReadList();
            break;

        case '\'': /* quote */
            buf++;
            return ReadQuote();
            break;

        case '-': /* minus num */
            buf++;
            return ReadMinusNumber();
            break;

        case '"':
            buf++;
            return ReadString();
            break;

        case '\n':
            break;

        case ' ':  case '\f': case '\r':
        case '\t': case '\v':    
            break;

        default:
            {
                int n = isdigit(*buf);
                if (n) {/* int */
                    return ReadNumber();
                } else {
                    /* symbol */
                    return ReadSymbol();
                }
            }
            break;
        }
        buf++;
    }
    return Qnil;
}

static int CheckList(char *buf)
{
    char *p = buf;
    int begin = 0, end = 0;

    while (*p != '\0') {
        switch (*p) {
        case ')':
            begin++;
            break;

        case '(':
            end++;
            break;
        }
        p++;
    }
    if (begin != end) {
        Lisp_printf("perse error\n");
        return 1;
    }
    return 0;
}

static void Read(char *str)
{
    LispObject r;
    
    buf = str;
    if (CheckList(buf)) return;
    
    for (r = Parse(); !NILP(r); r = Parse()) {
        PrintList(r);
        PrintList(Eval(r));
    }
}

void LoadFromFile(char *name)
{
    char *buf = ReadFile(name);
    Read(buf);
    if (buf == NULL) free(buf);
}

