#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include "lisp.h"

int Lisp_print(char *format, ...)
{
    int r;
    va_list args;

    va_start(args, format);
    r = vprintf(format, args);
    va_end(args);
    return r;
}

FILE *Lisp_fopen(char *name, char *mode)
{
    FILE *fp;
    fopen_s(&fp, name, mode);
    return fp;
}

void Lisp_fclose(FILE *fp)
{
    fclose(fp);
}

static FILE *debugLog = NULL;

void Lisp_InitLog(void)
{
    debugLog = Lisp_fopen("lisp_log.txt", "w");
}

int Lisp_WriteLog(char *format, ...)
{
    int r;
    va_list args;
    
    va_start(args, format);
    r = vfprintf(debugLog, format, args);
    va_end(args);
    return r;
}    

void Lisp_QuitLog(void)
{
    Lisp_fclose(debugLog);
}    

int Lisp_sprintf(char *buf, int size, char *fotmat, ...)
{
    int r;
    va_list arg;

    va_start(arg, fotmat);
    r = vsprintf_s(buf, size, fotmat, arg);
    va_end(arg);
    return r;
}

int Lisp_strcpy(char *buf1, int size, char *buf2)
{
    int r;
    r = strcpy_s(buf1, size, buf2);
    return r;
}

LispObject Lisp_exit(void)
{
    exit(0);
    return Qnil;
}





