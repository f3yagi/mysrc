#include "lisp.h"

int main(int argc, char **argv)
{
    if (argc < 2) {
        Lisp_printf("lisp.exe <file>\n");
        return -1;
    }
    InitGC();
    InitLisp();
    InitAlloc();
    InitEvel();
    InitMath();
    LoadFromFile(argv[1]);
    QuitGC();
    return 0;
}    
