#include <stdlib.h>
#include "lisp.h"

static int ListLen(LispObject o)
{
    int i = 0;

    for ( ; CONSP(o); o = CDR(o)) {
        i++;
    }
    return i;
}

static LispObject EvalArgs(LispObject arg, int n)
{
    if (!CONSP(arg)) {
        return arg;
    } else {
        LispObject r = Qnil;
        LispObject *argv = (LispObject *)malloc(sizeof(LispObject) * n);
        int i = n;
        
        for ( ; CONSP(arg); arg = CDR(arg)) {
            argv[--i] = Eval(CAR(arg));
        }
        for (i = 0; i < n; i++) {
            r = Cons(argv[i], r);
        }
        free(argv);
        return r;
    }
}

LispObject Apply(LispObject func, LispObject param, int n)
{
    LispObject r = Qnil, arg, defparm;
    int i;
    
    if (CONSP(func)) { /* lambda */
        arg     = CDR(CDR(func));
        defparm = CAR(CDR(func));
    } else if (SYMBOLP(func)) {/* defun */
        func    = GetStack(func);
        arg     = CDR(func);
        defparm = CAR(func);
    }
    for ( ; CONSP(defparm); defparm = CDR(defparm), param = CDR(param)) {
        PushTemporaryStack(CAR(defparm), CAR(param));
    }
    for ( ; CONSP(arg); arg = CDR(arg)) {
        r = Eval(CAR(arg));
    }
    for (i = 0; i < n; i++) {
        PopTemporaryStack();
    }
    return r;
}

LispObject Defun(LispObject symbol, LispObject args, LispObject body)
{
    CHECK_SYMBOL(symbol);
    PushStack(symbol, Cons2(args, Cons2(body, Qnil)));
    return symbol;
}

LispObject Eval(LispObject o)
{
    LispObject r = Qnil;
    
    switch (TYPE(o)) {
    case LISP_INTEGER: case LISP_FLOAT:
    case LISP_STRING:
        return o;
        break;

    case LISP_NIL:
        return Qnil;
        break;

    case LISP_TRUE:
        return Qtrue;

    case LISP_SYMBOL:
        return GetStack(o);
        break;
        
    case LISP_CONS:
        {
            LispObject name = CAR(o);
            LispObject arg  = CDR(o);
            
            /* eval defun */
            if (SYMBOLP(name)) {
                LispObject o = GetStack(name);
                if (!NILP(o)) {
                    int n = ListLen(arg);
                    LispObject param = EvalArgs(arg, n);
                    r = Apply(name, param, n);
                    goto end;
                }               
            }
            /*eval lambda */
            if (CONSP(name) && SYMBOLP(CAR(CAR(o)))) {
                int eq = SymbolEqual(CAR(CAR(o)), "lambda");
                if (eq) {
                    int n = ListLen(arg);
                    LispObject param = EvalArgs(arg, n);
                    r = Apply(name, param, n);
                    goto end;
                }
            }
            /* cfunction */
            if (SYMBOLP(name)) {
                struct LispCFunction *f = GetCFunction(LSYMBOL(name)->name);
                if (f == NULL) {
                    Lisp_printf("not defined %s\n", LSYMBOL(name)->name);
                    abort();
                }
                if (f != NULL) {
                    int i = 0, argc = ListLen(arg);
                    LispObject param, *argv;
                    
                    if (argc == 0) {
                        r = f->func();
                        goto end;
                    }
                    argv = (LispObject *)malloc(sizeof(LispObject) * argc);
                    if (f->evalArg) {
                        param = EvalArgs(arg, argc);
                    } else {
                        param = arg;
                    }
                    for ( ; CONSP(param); param = CDR(param)) {
                        argv[i++] = CAR(param);
                    }
                    switch (f->argc) {
                    case 1:
                        r = f->func(argv[0]);
                        break;

                    case 2:
                        r = f->func(argv[0], argv[1]);
                        break;

                    case 3:
                        r = f->func(argv[0], argv[1], argv[2]);
                        break;

                    case 4:
                        r = f->func(argv[0], argv[1], argv[2], argv[3]);
                        break;

                    case 5:
                        r = f->func(argv[0], argv[1], argv[2], argv[3], argv[4]);
                        break;

                    case 6:
                        r = f->func(argv[0], argv[1], argv[2], argv[3],
                                    argv[4], argv[5]);
                        break;

                    case 7:
                        r = f->func(argv[0], argv[1], argv[2], argv[3],
                                    argv[4], argv[5], argv[6]);
                        break;

                    case 8:
                        r = f->func(argv[0], argv[1], argv[2], argv[3],
                                    argv[4], argv[5], argv[6], argv[7]);
                        break;

                    case 9:
                        r = f->func(argv[0], argv[1], argv[2], argv[3],
                                    argv[4], argv[5], argv[6], argv[7], argv[8]);
                        break;
                        
                    case MANY_ARGS:
                        r = f->func(argv, argc);
                        break;

                    default:
                        Lisp_printf("args over %d\n", f->argc);                 
                        break;
                    }
                    free(argv);
                    goto end;
                }
            }
        }
        break;
    }
 end:
    return r;
}

void InitEvel(void)
{
    DEF_SUBR(Eval, "eval", 1, 1);
    DEF_SUBR(Defun,"defun", 3, 0);
}


