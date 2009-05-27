#include "lisp.h"
#ifdef WIN32
#define _USE_MATH_DEFINES
#endif
#include <math.h>

#define CHECK_NUMBER(o) if (!(FLOATP(o) || INTEGERP(o))) LISP_ERROR("type error\n")
#define GET_NUMBER(o) (FLOATP(o) ? LFLOAT(o)->value : LINTEGER(o))

#define FUNC1(func, args)			\
{						\
    double r;					\
    						\
    CHECK_NUMBER(args);				\
    r = func(GET_NUMBER(args));			\
    return MakeFloat(r);			\
}

#define FUNC2(func, args1, args2)			\
{						\
    double r;					\
    						\
    CHECK_NUMBER(args1);					\
    CHECK_NUMBER(args2);					\
    r = func(GET_NUMBER(args1), GET_NUMBER(args2));		\
    return MakeFloat(r);				\
}

static LispObject Cos(LispObject args)
{
    FUNC1(cos, args);
}

static LispObject Sin(LispObject args)
{
    FUNC1(sin, args);
}

static LispObject Tan(LispObject args)
{
    FUNC1(tan, args);
}

static LispObject ArcCos(LispObject args)
{
    FUNC1(acos, args);
}

static LispObject ArcSin(LispObject args)
{
    FUNC1(asin, args);
}

static LispObject ArcTan(LispObject args)
{
    FUNC1(atan, args);
}

static LispObject ArcTan2(LispObject args1, LispObject args2)
{
    FUNC2(atan2, args1, args2);
}

static LispObject Exp(LispObject args)
{
    FUNC1(exp, args);
}

static LispObject Sqrt(LispObject args)
{
    FUNC1(sqrt, args);
}

static LispObject Log(LispObject args)
{
    FUNC1(log, args);
}

static LispObject Log10(LispObject args)
{
    FUNC1(log10, args);
}

static LispObject ToRad(LispObject args)
{
    double r;
    
    CHECK_NUMBER(args);
    r = GET_NUMBER(args);
    r = M_PI / 180.0 * r;
    return MakeFloat(r);
}

static LispObject ToDeg(LispObject args)
{
    double r;
    
    CHECK_NUMBER(args);
    r = GET_NUMBER(args);
    r = 180.0 / M_PI * r;
    return MakeFloat(r);
}

static LispObject Abs(LispObject args)
{
    CHECK_NUMBER(args);
    if (FLOATP(args)) {
	double r = fabs(LFLOAT(args)->value);
	return MakeFloat(r);
    }
    if (INTEGERP(args)) {
	int r = abs(LINTEGER(args));
	return MakeInteger(r);
    }
    return Qnil;
}

static LispObject Pow(LispObject args1, LispObject args2)
{
    FUNC2(pow, args1, args2);
}

void InitMath(void)
{
    DEF_SUBR(Cos, "cos", 1, 1);
    DEF_SUBR(Sin, "sin", 1, 1);
    DEF_SUBR(Tan, "tan", 1, 1);
    DEF_SUBR(ArcCos, "acos", 1, 1);
    DEF_SUBR(ArcSin, "asin", 1, 1);    
    DEF_SUBR(ArcTan, "atan", 1, 1);
    DEF_SUBR(ArcTan2, "atan2", 2, 1);
    DEF_SUBR(Abs, "abs", 1, 1);
    DEF_SUBR(Pow, "pow", 2, 1);
    DEF_SUBR(Exp, "exp", 1, 1);
    DEF_SUBR(Sqrt, "sqrt", 1, 1);
    DEF_SUBR(Log, "log", 1, 1);
    DEF_SUBR(Log10, "log10", 1, 1);
    DEF_SUBR(ToDeg, "to-deg", 1, 1);
    DEF_SUBR(ToRad, "to-rad", 1, 1);
}

