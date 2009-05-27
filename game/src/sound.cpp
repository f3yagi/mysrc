#include <windows.h>
#include <dsound.h>
#include "lib.h"
#include "engine.h"

/* direct sound */
#define RELEASE(p) {if(p != NULL) {p->Release(); p = NULL;}}

LPDIRECTSOUND8      lpDS  = NULL;
LPDIRECTSOUNDBUFFER lpDSP = NULL; /* primari buffer */

extern HWND hWnd;
static HINSTANCE dsoundDLL;
typedef HRESULT (__stdcall *pDirectSoundCreate8)(LPCGUID lpcGuidDevice, LPDIRECTSOUND8 * ppDS8, LPUNKNOWN  pUnkOuter);

static int InitDSound(HWND hWnd)
{
    HRESULT hr;
    pDirectSoundCreate8 SoundCreate;    

    dsoundDLL = LoadLibrary("dsound.dll");
    if (dsoundDLL == NULL) {
        WriteLog("not find dsound.dll");
    }
    SoundCreate = (pDirectSoundCreate8)GetProcAddress(dsoundDLL, "DirectSoundCreate8");
    hr = SoundCreate(NULL, &lpDS, NULL);
    if (FAILED(hr)) {
        goto end;
    }
    hr = lpDS->SetCooperativeLevel(hWnd, DSSCL_PRIORITY);
    if (FAILED(hr)) {
        goto end;
    }
    DSBUFFERDESC desc;
    ZeroMemory(&desc,sizeof(DSBUFFERDESC));
    desc.dwSize = sizeof(DSBUFFERDESC);
    desc.dwFlags = DSBCAPS_PRIMARYBUFFER;
    hr = lpDS->CreateSoundBuffer(&desc, &lpDSP, NULL);
    if (FAILED(hr)) {
        goto end;
    }
    WriteLog("init sound\n");
    return 1;
 end:
    return 0;
}

static LPDIRECTSOUNDBUFFER OpenSoundFile(char *name)
{
    DSBUFFERDESC DSBufDESC;
    WAVEFORMATEX waveFormat;
    void* buf;
    DWORD size;
    HMMIO hmmio;
    MMCKINFO cParent, cSub;
    LPDIRECTSOUNDBUFFER lpDSBuf = NULL;
    
    MMIOINFO mem;
    int fileSize;

    char *data = LoadFile(name, &fileSize);
    
    memset(&mem, 0, sizeof(MMIOINFO));
    mem.pchBuffer = data;
    mem.cchBuffer = fileSize;
    mem.fccIOProc = FOURCC_MEM;
    hmmio = mmioOpen(NULL, &mem, MMIO_ALLOCBUF | MMIO_READ);

    cParent.fccType = mmioFOURCC('W','A','V','E');
    if (mmioDescend(hmmio, &cParent, NULL, MMIO_FINDRIFF) != MMSYSERR_NOERROR) {
        return NULL;
    }
    cSub.ckid = mmioFOURCC('f','m','t',' ');
    if (mmioDescend(hmmio, &cSub, &cParent, MMIO_FINDCHUNK) != MMSYSERR_NOERROR) {
        goto end;
    }
    mmioRead(hmmio, (char*)&waveFormat, cSub.cksize);
    mmioAscend(hmmio, &cSub, 0);
    if (waveFormat.wFormatTag != WAVE_FORMAT_PCM) {
        goto end;
    }
    cSub.ckid = mmioFOURCC('d','a','t','a');
    if (mmioDescend(hmmio, &cSub, &cParent, MMIO_FINDCHUNK) != MMSYSERR_NOERROR) {
        goto end;
    }
    ZeroMemory(&DSBufDESC, sizeof(DSBUFFERDESC));
    DSBufDESC.dwSize = sizeof(DSBUFFERDESC);
    DSBufDESC.dwFlags = DSBCAPS_CTRLPAN|DSBCAPS_CTRLVOLUME|DSBCAPS_GLOBALFOCUS;
    DSBufDESC.dwBufferBytes = cSub.cksize;
    DSBufDESC.lpwfxFormat = &waveFormat;
    
    lpDS->CreateSoundBuffer(&DSBufDESC, &lpDSBuf, NULL);
    lpDSBuf->Lock(0, cSub.cksize, &buf, &size, NULL, 0, 0);
    mmioRead(hmmio, (HPSTR)buf, (LONG)cSub.cksize);
    lpDSBuf->Unlock(buf, size, NULL, 0);
    mmioClose(hmmio, MMIO_FHOPEN);
    return lpDSBuf;
 end:
    mmioClose(hmmio, MMIO_FHOPEN);
    free(data);
    return NULL;
}

struct Sbuf {
    char busy;
    LPDIRECTSOUNDBUFFER buf;
};
#define MAX_SBUF_SIZE 256
static struct Sbuf sbuf[MAX_SBUF_SIZE];

#define CHECK_VALUE(i)                                                  \
    do {                                                                \
        if ( 0 > i || i > MAX_SBUF_SIZE) {                              \
            return;                                                     \
        }                                                               \
        if (sbuf[i].busy == 0) {                                        \
            return;                                                     \
        }                                                               \
    } while (0)   

static int SbufNew()
{
    int i;

    for (i = 0; i < MAX_SBUF_SIZE; i++) {
        if (!sbuf[i].busy) {
            return i;
        }
    }
    EngineError("sbuf error\n");
    return -1;
}    

int load_sound(char *name)
{
    int n = SbufNew();
    sbuf[n].busy = 1;
    sbuf[n].buf  = OpenSoundFile(name);
    return n;
}

int load_sound_dummy(char *name)
{
    return 0;
}

void FreeSound(int id)
{
    CHECK_VALUE(id);
    sbuf[id].busy = 0;
    RELEASE(sbuf[id].buf);
}    

void LoopSound(int id)
{
    CHECK_VALUE(id);
    sbuf[id].buf->SetCurrentPosition(0);
    sbuf[id].buf->Play(0, 0, DSBPLAY_LOOPING);
}    

void SoundPlay(int id)
{
    CHECK_VALUE(id);
    sbuf[id].buf->Play(0, 0, 0);
}

void StopSound()
{
    int i;
    
    for (i = 0; i < MAX_SBUF_SIZE; i++) {
        if (sbuf[i].busy) {
            sbuf[i].buf->Stop();
        }
    }
}

void EndSound()
{
    int i;

    for (i = 0; i < MAX_SBUF_SIZE; i++) {
        if (sbuf[i].busy) {        
            sbuf[i].buf->SetCurrentPosition(0);
            sbuf[i].buf->Stop();
        }
    }
}

static void QuitSound()
{
    int i;
    
    for (i = 0; i < MAX_SBUF_SIZE; i++) {
        if (sbuf[i].busy) {
            RELEASE(sbuf[i].buf);
            sbuf[i].busy = 0;
        }
    }
    RELEASE(lpDSP);
    RELEASE(lpDS);
    FreeLibrary(dsoundDLL);
    WriteLog("quit sound\n");
}

int (*LoadSound)(char *name);

int InitSound(int on)
{
    int r;

    AddFinalize(QuitSound);
    ZeroMemory(&sbuf, sizeof(struct Sbuf) * MAX_SBUF_SIZE);
    if (on) {
        LoadSound = load_sound;
    } else {
        LoadSound = load_sound_dummy;
    }
    r = InitDSound(hWnd);
    if (!r) {
        LoadSound = load_sound_dummy;        
    }
    return 1;
}
