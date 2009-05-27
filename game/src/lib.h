#ifndef MAX_PATH
#define MAX_PATH 260
#endif

int Atoi(char *str);
double Atof(char *str);
double Fabs(double n);
int SPrintf(char *buf, int size, char *format, ...);
int StrCpy(char *str1, int size, char *str2);
int StrCmp(char *str1, char *str2);
void StrCat(char *str1, int size, char *str2);
void MemSet(void *mem, int c, int size);
void *MemCpy(void *buf1, void *buf2, int size);
void DebugName(char *buf, char *fullPath);
int GetOSVersion(void);
char *GetExt(char *path);
void RemoveExt(char *buf, char *path);
int OpenFileDialog(char *name, char *filter);
int SaveFileDialog(char *name, char *filter);
void EngineSleep(int num);
void *Malloc(int size);
void Free(void *mem);
void AddFinalize(void (*Fin)());

enum PrintType {
    PRINT_LOG_FILE,
    PRINT_WINDOW,
    PRINT_VS_WINDOW,
    PRINT_MESSAGEBOX
};
void Printf(enum PrintType type, void (*Finalize)(), char *format, ...);
#define PrintWindow(format, ...) Printf(PRINT_WINDOW, NULL, format, __VA_ARGS__)
#define PrintMessageBox(format, ...) Printf(PRINT_MESSAGEBOX, NULL, format, __VA_ARGS__)

#define DebugLog(finalize, format, ...)                           \
    do {                                                                \
        char source[MAX_PATH];                                          \
        DebugName(source, __FILE__);                                    \
        Printf(PRINT_LOG_FILE, finalize, "%s %d:"format,source, __LINE__, __VA_ARGS__); \
    } while (0)

#define WriteLog(format, ...)  DebugLog(NULL, format, __VA_ARGS__) 
#define EngineError(format, ...) DebugLog(QuitSystem, format, __VA_ARGS__) 

void InitSystem();
void QuitSystem();
