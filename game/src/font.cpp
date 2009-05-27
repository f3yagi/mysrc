#include <stdio.h>
#include <stdarg.h>
#include "engine.h"
#include "lib.h"

struct FontRes {
    int c_0;
    int c_1;
    int c_2;
    int c_3;
    int c_4;
    int c_5;
    int c_6;
    int c_7;
    int c_8;
    int c_9;
    /*----------------*/
    int c_A;
    int c_B;
    int c_C;
    int c_D;
    int c_E;
    int c_F;
    int c_G;
    int c_H;
    int c_I;
    int c_J;
    int c_K;
    int c_L;
    int c_M;
    int c_N;
    int c_O;
    int c_P;
    int c_Q;
    int c_R;
    int c_S;
    int c_T;
    int c_U;
    int c_V;
    int c_W;
    int c_X;
    int c_Y;
    int c_Z;
    /*----------------*/
    int c_a;
    int c_b;
    int c_c;
    int c_d;
    int c_e;
    int c_f;
    int c_g;
    int c_h;
    int c_i;
    int c_j;
    int c_k;
    int c_l;
    int c_m;
    int c_n;
    int c_o;
    int c_p;
    int c_q;
    int c_r;
    int c_s;
    int c_t;
    int c_u;
    int c_v;
    int c_w;
    int c_x;
    int c_y;
    int c_z;
};
static struct FontRes font;

void InitFont()
{
    font.c_0 = GetTextrueObject("font_0");
    font.c_1 = GetTextrueObject("font_1");
    font.c_2 = GetTextrueObject("font_2");
    font.c_3 = GetTextrueObject("font_3");
    font.c_4 = GetTextrueObject("font_4");
    font.c_5 = GetTextrueObject("font_5");
    font.c_6 = GetTextrueObject("font_6");
    font.c_7 = GetTextrueObject("font_7");
    font.c_8 = GetTextrueObject("font_8");
    font.c_9 = GetTextrueObject("font_9");
    font.c_x = GetTextrueObject("font_x");
}    

void FontPrintf(float x, float y, char *format, ...)
{
#define DRAW_FONT(n) DrawTextureObject(font.c_##n, x, y); break; 
    
    char buf[MAX_PATH];
    va_list args;
    int i = 0;

    va_start(args, format) ;
    vsprintf_s(buf, MAX_PATH, format, args);
    va_end(args);
    
    while (buf[i] != '\0') {
        switch (buf[i]) {
        case '0':
            DRAW_FONT(0)
        case '1':
            DRAW_FONT(1)
        case '2':
            DRAW_FONT(2)
        case '3':
            DRAW_FONT(3)
        case '4':
            DRAW_FONT(4)
        case '5':
            DRAW_FONT(5)
                
        case '6':
            DRAW_FONT(6)
        case '7':
            DRAW_FONT(7)
        case '8':
            DRAW_FONT(8)
        case '9':
            DRAW_FONT(9)
            
        case 'A':
            DRAW_FONT(A)
        case 'B':
            DRAW_FONT(B)
        case 'C':
            DRAW_FONT(C)
        case 'D':
            DRAW_FONT(D)
        case 'E':
            DRAW_FONT(E)
        case 'F':
            DRAW_FONT(F)
        case 'G':
            DRAW_FONT(G)
        case 'H':
            DRAW_FONT(H)
        case 'I':
            DRAW_FONT(I)
        case 'J':
            DRAW_FONT(J)
        case 'K':
            DRAW_FONT(K)
        case 'L':
            DRAW_FONT(L)
        case 'M':
            DRAW_FONT(M)
        case 'N':
            DRAW_FONT(N)
        case 'O':
            DRAW_FONT(O)
        case 'P':
            DRAW_FONT(P)
        case 'Q':
            DRAW_FONT(Q)
        case 'R':
            DRAW_FONT(R)
        case 'S':
            DRAW_FONT(S)
        case 'T':
            DRAW_FONT(T)
        case 'U':
            DRAW_FONT(U)
        case 'V':
            DRAW_FONT(V)
        case 'W':
            DRAW_FONT(W)
        case 'X':
            DRAW_FONT(X)
        case 'Y':
            DRAW_FONT(Y)
        case 'Z':
            DRAW_FONT(Z)
            
        case 'a':
            DRAW_FONT(a)
        case 'b':
            DRAW_FONT(b)
        case 'c':
            DRAW_FONT(c)
        case 'd':
            DRAW_FONT(d)
        case 'e':
            DRAW_FONT(e)
        case 'f':
            DRAW_FONT(f)
        case 'g':
            DRAW_FONT(g)
        case 'h':
            DRAW_FONT(h)
        case 'i':
            DRAW_FONT(i)
        case 'j':
            DRAW_FONT(j)
        case 'k':
            DRAW_FONT(k);
        case 'l':
            DRAW_FONT(l);
        case 'm':
            DRAW_FONT(m);
        case 'n':
            DRAW_FONT(n);
        case 'o':
            DRAW_FONT(o);
        case 'p':
            DRAW_FONT(p);
        case 'q':
            DRAW_FONT(q);
        case 'r':
            DRAW_FONT(r);
        case 's':
            DRAW_FONT(s);
        case 't':
            DRAW_FONT(t);
        case 'u':
            DRAW_FONT(u);
        case 'v':
            DRAW_FONT(v);
        case 'w':
            DRAW_FONT(w);
        case 'x':
            DRAW_FONT(x);
        case 'y':
            DRAW_FONT(y);
        case 'z':
            DRAW_FONT(z);

        case ' ':
            break;

        default:
            EngineError("not defined %c\n", buf[i]);
            break;
        }
        x += 16;
        i++;
    }
}





