#include "lisp.h"

/* global variable */
LispObject  Qtrue, Qnil;

static void Print(LispObject o)
{
    switch (TYPE(o)) {
    case LISP_CONS:
	    Lisp_printf("(");
	    Print(CAR(o));
	    while (CONSP(CDR(o))) {
		Lisp_printf(" ");
		o = CDR(o);
		Print(CAR(o));
	    }
	    if (!NILP(CDR(o))) {
		Lisp_printf(" . ");
		Print(CDR(o));		
	    }
	    Lisp_printf(")");		
	break;

    case LISP_SYMBOL:
	Lisp_printf("%s", LSYMBOL(o)->name); 
	break;
    
    case LISP_INTEGER:
	Lisp_printf("%ld", LINTEGER(o));
	break;

    case LISP_FLOAT:
	Lisp_printf("%f", LFLOAT(o)->value);
	break;

    case LISP_STRING:
	Lisp_printf("\"%s\"", LSTRING(o)->str);
	break;

    case LISP_TRUE:
	Lisp_printf("t");
	break;

    case LISP_NIL:
	Lisp_printf("nil");
	break;
    }
}

LispObject PrintList(LispObject o)
{
    Print(o);
    Lisp_printf("\n");
    return o;
}

static void MakeLogicalSymbol(void)
{
    TYPE(Qtrue) = LISP_TRUE;
    TYPE(Qnil)  = LISP_NIL;
}

LispObject Cons(LispObject car, LispObject cdr)
{
    LispObject o;
    return CONS(o, car, cdr);
}

static LispObject Car(LispObject o)
{
    return CAR(o);
}

static LispObject Cdr(LispObject o)
{
    return CDR(o);
}

static LispObject List(LispObject *args, int n)
{
    LispObject r = Qnil;
    while (n > 0) {
	r = Cons(args[--n], r);
    }
    return r;
}

#define OP_CALC(OP)				\
{						\
    int i, ri = 0;				\
    double rf = 0.0;				\
    int fmode = 0;				\
    						\
    for (i = 0; i < n; i++) {				\
	if (!INTEGERP(args[i]) && !FLOATP(args[i])) {	\
	    return Qnil;				\
	}						\
	if (FLOATP(args[i])) fmode = 1;			\
    }							\
    if (fmode) goto floatMode;				\
    ri = LINTEGER(args[0]);				\
    for (i = 1; i < n; i++) {				\
	ri OP= LINTEGER(args[i]);			\
    }							\
    return MakeInteger(ri);				\
    							\
 floatMode:						\
    if (INTEGERP(args[0])) {				\
	rf = LINTEGER(args[0]);				\
    } else {						\
	rf = LFLOAT(args[0])->value;			\
    }							\
    for (i = 1; i < n; i++) {				\
	if (INTEGERP(args[i])) {			\
	    rf OP= LINTEGER(args[i]);			\
	} else {					\
	    rf OP= LFLOAT(args[i])->value;		\
	}						\
    }							\
    return MakeFloat(rf);				\
}

static LispObject OpPlus(LispObject *args, int n)
{
    OP_CALC(+);
}

static LispObject OpMinus(LispObject *args, int n)
{
    OP_CALC(-);
}
static 
LispObject OpDiv(LispObject *args, int n)
{
    OP_CALC(/);
}

static LispObject OpMulti(LispObject *args, int n)
{
    OP_CALC(*);
}

static LispObject Progn(LispObject *args, int n)
{
    LispObject r;
    int i;

    for (i = 0; i < n; i++) {
	r = Eval(args[i]);
    }
    return r;
}

static LispObject Setq(LispObject symbol, LispObject value)
{
    LispObject r;
    
    value = Eval(value);
    r = SearchSetStack(symbol, value);
    if (NILP(r)) {
	value = CopyObject(value);
	PushStack(symbol, value);
    }
    return value;
}

static LispObject Let(LispObject *args, int n)
{
    int i, c = 0;
    LispObject r = Qnil;
    LispObject var = args[0];

    for ( ; CONSP(var); var = CDR(var)) {
	CHECK_SYMBOL(CAR(var));
	PushTemporaryStack(CAR(var), Qnil);
	c++;
    }
    for (i = 1; i < n; i++) {
	r = Eval(args[i]);
    }
    for (i = 0; i < c; i++) {
	PopTemporaryStack();
    }
    return r;
}

static LispObject Quote(LispObject args)
{
    return args;
}

static LispObject If(LispObject *args, int n)
{
    int i;
    LispObject r = Qnil;
    
    if (TRUEP(Eval(args[0]))) {
	return Eval(args[1]);
    }
    /* else */
    for (i = 2; i < n; i++) {
	r = Eval(args[i]);
    }
    return r;
}

static LispObject Cond(LispObject *args, int n)
{
    int i;
    
    for (i = 0; i < n; i++) {
	LispObject flag = CAR(args[i]);
	LispObject body = CAR(CDR(args[i]));
	if (TRUEP(Eval(flag))) {
	    return Eval(body);
	}
    }
    return Qnil;
}

static LispObject While(LispObject *args, int n)
{
    int i;
    LispObject r = Qnil;
    
    while (TRUEP(Eval(args[0]))) {
	for (i = 1; i < n; i++) {
	    r = Eval(args[i]);
	}
    }
    return r;
}

static LispObject Not(LispObject o)
{
    if (NILP(o)) {
	return Qtrue;
    }
    return Qnil;
}

#define COMPARE_NUMBER(arg1, arg2, OP)					\
{									\
    if (FLOATP(arg1) && FLOATP(arg2)) {					\
	return (LFLOAT(arg1)->value OP LFLOAT(arg2)->value) ? Qtrue : Qnil; \
    }									\
    if (INTEGERP(arg1) && INTEGERP(arg2)) {				\
	return (LINTEGER(arg1) OP LINTEGER(arg2)) ? Qtrue : Qnil;	\
    }									\
    if (INTEGERP(arg1) && FLOATP(arg2)) {				\
	return (LINTEGER(arg1) OP LFLOAT(arg2)->value) ? Qtrue : Qnil;	\
    }									\
    if (FLOATP(arg1) && INTEGERP(arg2)) {				\
	return (LFLOAT(arg1)->value OP LINTEGER(arg2)) ? Qtrue : Qnil;	\
    }									\
    LISP_ERROR("type error\n");						\
}
    
static LispObject Eq(LispObject arg1, LispObject arg2)
{
    if (STRINGP(arg1) && STRINGP(arg2)) {
	return (StrEqual(LSTRING(arg1)->str, LSTRING(arg2)->str)) ? Qtrue : Qnil;
    }
    COMPARE_NUMBER(arg1, arg2, ==);
    return Qnil;
}

static LispObject Lss(LispObject arg1, LispObject arg2)
{
    /* < */    
    COMPARE_NUMBER(arg1, arg2, <);
    return Qnil;
}

static LispObject Leq(LispObject arg1, LispObject arg2)
{
    /* <= */    
    COMPARE_NUMBER(arg1, arg2, <=);
    return Qnil;
}

static LispObject Gtr(LispObject arg1, LispObject arg2)
{
    /* > */    
    COMPARE_NUMBER(arg1, arg2, >);
    return Qnil;
}

static LispObject Geq(LispObject arg1, LispObject arg2)
{
    /* >= */    
    COMPARE_NUMBER(arg1, arg2, >=);
    return Qnil;
}

void InitLisp(void)
{
    MakeLogicalSymbol();
    DEF_SUBR(Car, "car", 1, 1);
    DEF_SUBR(Cdr, "cdr", 1, 1);
    DEF_SUBR(Cons, "cons", 2, 1);
    DEF_SUBR(List, "list", MANY_ARGS, 1);
    DEF_SUBR(Progn,"progn", MANY_ARGS, 0);
    DEF_SUBR(Setq, "setq", 2, 0);
    DEF_SUBR(Let, "let", MANY_ARGS, 0);
    DEF_SUBR(OpPlus,"+", MANY_ARGS, 1);
    DEF_SUBR(OpMinus, "-", MANY_ARGS, 1);
    DEF_SUBR(OpDiv, "/", MANY_ARGS, 1);
    DEF_SUBR(OpMulti, "*", MANY_ARGS, 1);
    DEF_SUBR(PrintList, "print", 1, 1);
    
    DEF_SUBR(Quote, "quote", 1, 0);
    DEF_SUBR(If, "if", MANY_ARGS, 0);
    DEF_SUBR(Cond, "cond", MANY_ARGS, 0);
    DEF_SUBR(While, "while", MANY_ARGS, 0);
    DEF_SUBR(Not, "!", 1, 1);
    DEF_SUBR(Eq,  "eq", 2, 1);
    DEF_SUBR(Eq,  "=", 2, 1);
    DEF_SUBR(Lss, "<", 2, 1);
    DEF_SUBR(Leq, "<=", 2, 1);
    DEF_SUBR(Gtr, ">", 2, 1);
    DEF_SUBR(Geq, ">=", 2, 1);
}

