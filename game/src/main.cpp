#include <windows.h>
#include "engine.h"
#include "lib.h"
#include "action.h"
#include "conf.h"

/* global */
void (*GameMain)();
void (*DrawMain)();
struct Win32Input winput;

static int end = 0;
static char fullscreen = 0;

#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

static void InitFps()
{
    timeBeginPeriod(1);
}

static void QuitFps()
{
    timeEndPeriod(1);
}

void SearchFps() {
    static DWORD count;
    static DWORD beforeTime = timeGetTime();
    DWORD nowTime = timeGetTime();

    if(nowTime - beforeTime >= 1000){
        beforeTime = nowTime;
        engineFps = (int)count;
        count = 0;
    }
    count++;
}

void QuitMainLoop()
{
    end = 1;
}

#define CALL_FUNC(func) if (func != NULL) func()

void IsNextStage(); /* player.cpp */

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
    case WM_CLOSE:
        end = 1;
        return 1;
        break;

    case WM_KEYDOWN:
        switch (wParam) {
        case VK_ESCAPE:
            end = 1;
            return 1;
            break;

        case 'I':
            IsNextStage();
            break;

        case 'Z':
            CALL_FUNC(winput.KeyZ);
            return 1;
            break;
            
        case 'X':
            CALL_FUNC(winput.KeyX);
            return 1;
            break;

        case 'C':
            CALL_FUNC(winput.KeyC);
            return 1;
            break;

        case 'A':
            CALL_FUNC(winput.KeyA);
            return 1;
            break;

        case 'S':
            CALL_FUNC(winput.KeyS);
            return 1;
            break;

        case 'D':
            CALL_FUNC(winput.KeyD);
            return 1;
            break;

        case 'W':
            CALL_FUNC(winput.KeyW);            
            return 1;
            break;

        case 'Q':
            CALL_FUNC(winput.KeyQ);
            return 1;
            break;

        case 'F':
            CALL_FUNC(winput.KeyF);
            return 1;
            break;

        case 'E':
            CALL_FUNC(winput.KeyE);
            return 1;
            break;
            
        case 'R':
            CALL_FUNC(winput.KeyR);
            return 1;
            break;

        case 'T':
            CALL_FUNC(winput.KeyT);
            return 1;
            break;
        }
        break;
    }
    return DefWindowProc(hWnd, message, wParam, lParam);    
}

/* global variable */
HWND hWnd; 
HINSTANCE hInst;
int engineFps;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow)
{
    int r;
    MSG msg;
    WNDCLASS winc;

    InitSystem();              /* start log */
    AddFinalize(QuitFps);
    
    hInst              = hInstance;
    winc.style         = CS_HREDRAW | CS_VREDRAW;
    winc.lpfnWndProc   = WndProc;
    winc.cbClsExtra    = 0;
    winc.cbWndExtra    = 0;
    winc.hInstance     = hInstance;
    winc.hIcon         = LoadIcon(NULL , IDI_APPLICATION);
    //winc.hIcon         = LoadIcon(winc.hInstance, "TITLE");

    winc.hCursor       = LoadCursor(NULL , IDC_ARROW);
    winc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    winc.lpszMenuName  = NULL;
    winc.lpszClassName = "yagui3";
#ifndef _DEBUG    
    r = MessageBox(NULL , "フルスクリーンで起動しますか", "起動", MB_YESNO);
#else
    r = IDNO;
#endif    
    if (r == IDYES) {
        fullscreen = 1;
    } else {
        fullscreen = 0;
    }
    if (!RegisterClass(&winc)) {
        WriteLog("error: RegisterClass\n");
        goto end;
    }
    if (fullscreen) {
        hWnd   = CreateWindow(winc.lpszClassName, "ぷろとたいぷ",
                              WS_VISIBLE | WS_POPUP,
                              0, 0, ENGINE_WIDTH, ENGINE_HEIGTH, NULL, NULL,
                              hInst, NULL);
        ShowCursor(FALSE);
    } else {
        int width, height;            
        width  = ENGINE_WIDTH  + GetSystemMetrics(SM_CXDLGFRAME) * 2;
        height = ENGINE_HEIGTH + GetSystemMetrics(SM_CYDLGFRAME) * 2 + GetSystemMetrics(SM_CYCAPTION);
        hWnd   = CreateWindow(winc.lpszClassName, "ぷろとたいぷ",
                              WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX | WS_VISIBLE,
                              100, 100, width, height, NULL, NULL,
                              hInst, NULL);
    }
    if (!hWnd) {
        WriteLog("error: hWnd null\n");
        goto end;
    }
    if (!InitVideo(fullscreen, ENGINE_WIDTH, ENGINE_HEIGTH)) {
        WriteLog("error: init d3d\n");
        goto end;
    }
    if (!InitInput()) {
        WriteLog("error: init dinput \n");
        goto end;
    }
#if defined _DEBUG || MAP_EDIT_MODE
    if (!InitSound(0)) {
        WriteLog("error: init sound \n");
    }
#else
    if (!InitSound(1)) {
        WriteLog("error: init sound \n");
    }
#endif
    InitResource();
    InitFps();
    OptionParse(__argc, __argv);
    
    DWORD interval = 50;    // 16.666...*3 
    DWORD preFrame = timeGetTime() * 3;
    while (!end) {
        if (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)) {
            if (!GetMessage(&msg, NULL, 0, 0)) break;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        while (timeGetTime() * 3 - preFrame < interval); /* busy loop */
        preFrame += interval;
        SearchFps();
        ActionMain();
        DrawEngine();
        ReadInput();
    }
 end:
    QuitSystem();
    return 0;
}



