#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <windows.h>
#include "lib.h"

void DebugName(char *buf, char *fullPath)
{
    char tmp[MAX_PATH];
    char name[MAX_PATH];
    char ext[MAX_PATH];
    
    _splitpath_s(fullPath,
                 NULL , NULL,
                 NULL,  NULL,
                 name , MAX_PATH,
                 ext, MAX_PATH);
    SPrintf(tmp, MAX_PATH, "%s%s", name, ext);
    StrCpy(buf, MAX_PATH, tmp);
}

int Atoi(char *str)
{
    return atoi(str);
}

double Atof(char *str)
{
    return atof(str);
}

double Fabs(double n)
{
    return fabs(n);
}    

void *Malloc(int size)
{
    return malloc(size);
}

void Free(void *mem)
{
    free(mem);
    mem = NULL;
}    

FILE *FOpen(char *name, char *mode)
{
    FILE *fp;
    fopen_s(&fp, name, mode);
    return fp;
}

void FClose(FILE *fp)
{
    fclose(fp);
}    

int SPrintf(char *buf, int size, char *format, ...)
{
    int r;
    va_list args;
    
    va_start(args, format);
    r = vsprintf_s(buf, MAX_PATH, format, args);
    va_end(args);
    return r;
}

int StrCpy(char *str1, int size, char *str2)
{
    int r;
    r = strcpy_s(str1, size, str2);
    return r;
}

int StrCmp(char *str1, char *str2)
{
    return strcmp(str1, str2);
}

void StrCat(char *str1, int size, char *str2)
{
    strcat_s(str1, size, str2);
}    

void MemSet(void *mem, int c, int size)
{
    memset(mem, c, size);
}

void *MemCpy(void *buf1, void *buf2, int size)
{
    return memcpy(buf1, buf2, size);
}

static FILE *debugLog = NULL;
extern HWND hWnd; 
extern HINSTANCE hInst;

void Printf(enum PrintType type, void (*Finalize)(), char *format, ...)
{
    switch (type) {
    case PRINT_LOG_FILE:
        {
            va_list args;
            va_start(args, format);
            vfprintf(debugLog, format, args);
            va_end(args);
        }
        break;
        
    case PRINT_VS_WINDOW:
        {
            char buf[MAX_PATH];
            va_list args;
            va_start(args, format);
            vsprintf_s(buf, MAX_PATH, format, args);
            va_end(args);
            OutputDebugString(buf);
        }
        break;

    case PRINT_WINDOW:
        {
            char buf[MAX_PATH];
            va_list args;
            va_start(args, format);
            vsprintf_s(buf, MAX_PATH, format, args);
            va_end(args);
            SetWindowText(hWnd, buf);
        }
        break;

    case PRINT_MESSAGEBOX:
        {
            char buf[MAX_PATH];
            va_list args;
            va_start(args, format);
            vsprintf_s(buf, MAX_PATH, format, args);
            va_end(args);
            MessageBox(hWnd, buf, "message", MB_OK);
        }
        break;
    }
    if (Finalize != NULL) {
        Finalize();
        exit(0);
    }
}

int GetOSVersion()
{
  OSVERSIONINFOEX os;
  ZeroMemory(&os, sizeof(OSVERSIONINFOEX));
  os.dwOSVersionInfoSize = sizeof(os);
  GetVersionEx((LPOSVERSIONINFO)&os);
    
    if (os.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
      return 98;

    if (os.dwPlatformId == VER_PLATFORM_WIN32_NT) {
        if (os.dwMajorVersion == 4) return 4;
	
	if (os.dwMajorVersion == 5) {
	  switch (os.dwMinorVersion) {
	    case 0:
	      /* windows2000 */
	      return 5;
	      break;
	      
	    case 1:
	      if (os.wSuiteMask == VER_SUITE_PERSONAL) {
		/* windowsXP home*/
		return 510;
              } else {
		/* windowsXP pro*/
		return 511;
	      }
	      break;
	  }
        }
	/* vista */	
	if (os.dwMinorVersion == 6) return 6;
	
	/* windowsNT */ 
	return 4;
    }
    /* unkown*/
    return 0;
}

int OpenFileDialog(char *name, char *filter)
{
    OPENFILENAME ofn;
    char tmp[MAX_PATH];
    
    ZeroMemory(tmp, sizeof(char) * MAX_PATH);
    ZeroMemory(&ofn, sizeof(OPENFILENAME));
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.lpstrFilter = filter;
    ofn.lpstrFile = tmp;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_FILEMUSTEXIST;
    if (GetOpenFileName(&ofn)) {
        StrCpy(name, MAX_PATH, tmp);        
        return 1;
    }
    return 0;
}

int SaveFileDialog(char *name, char *filter)
{
    OPENFILENAME ofn;
    char tmp[MAX_PATH];
    
    ZeroMemory(tmp, sizeof(char) * MAX_PATH);
    ZeroMemory(&ofn, sizeof(OPENFILENAME));
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.lpstrFilter = filter;
    ofn.lpstrFile = tmp;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_FILEMUSTEXIST;
    if (GetSaveFileName(&ofn)) {
        StrCpy(name, MAX_PATH, tmp);        
        return 1;
    }
    return 0;
}    

#include "shlwapi.h"
#pragma comment(lib, "shlwapi.lib")

char *GetExt(char *path)
{
    return PathFindExtension(path);
}    

void RemoveExt(char *buf, char *path)
{
    char tmp[MAX_PATH];
    
    strcpy_s(tmp, MAX_PATH, path);
    PathRemoveExtension(tmp);
    strcpy_s(buf, MAX_PATH, tmp);
}

static void WriteOsVer()
{
    switch (GetOSVersion()) {
    case 0:
        WriteLog("unkown os\n");
        break;
        
    case 4:
        WriteLog("windows nt\n");
        break;
        
    case 5:
        WriteLog("windows 2000\n");
        break;

    case 510: case 511:
        WriteLog("windows xp\n");
        break;

    case 6:
        WriteLog("windows vista\n");
        break;

    case 98:
        WriteLog("windows 98\n");
        break;
    }
}    

#define MAX_FINALIZE 256
void (*Finalize[MAX_FINALIZE])();
static int finNum = 0;

void AddFinalize(void (*Fin)())
{
    Finalize[finNum] = Fin;
    finNum++;
}

static void QuitFinalize()
{
    int i;
    
    for (i = 0; i < finNum; i++) {
        Finalize[i]();
    }
    finNum = 0;
}    

#define LOG_FILE_NAME "log.txt"

void InitSystem()
{
    debugLog = FOpen(LOG_FILE_NAME, "w");
    if (debugLog == NULL) {
        exit(0);
    }
    WriteOsVer();
}

void QuitSystem()
{
    QuitFinalize();
    WriteLog("quit log\n");
    fclose(debugLog);
}
