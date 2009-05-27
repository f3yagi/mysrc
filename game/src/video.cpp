#pragma comment(lib, "d3dx8.lib")

#include <d3d8.h>
#include <d3dx8.h>
#include "lib.h"
#include "engine.h"

#define SAFE_RELEASE(p) {if(p != NULL) {p->Release(); p = NULL;}}

extern HWND hWnd; 
extern HINSTANCE hInst;

/* direct 3d */
static LPDIRECT3DDEVICE8 pd3dDevice   = NULL;
static LPDIRECT3DSURFACE8 pBackBuffer = NULL;
static LPDIRECT3D8 pD3D = NULL;
static D3DPRESENT_PARAMETERS d3dpp;

void SetDefaultRenderState()
{
  /* set alpha */
  //pd3dDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);
  //pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
  //pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);

  pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
  pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP , D3DTOP_MODULATE);
  pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
  pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
  pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP , D3DTOP_MODULATE);
  pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE); 

  pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
  pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
  pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
}

typedef IDirect3D8 *(__stdcall *pDirect3DCreate8)(UINT SDKVersion);
static HINSTANCE d3dDLL = NULL;

static int InitD3D(HWND hWnd, int fullScreen, int width, int height)
{
  HRESULT hr;
  pDirect3DCreate8 Direct3DCreate8;

  d3dDLL = LoadLibrary("d3d8.dll");
  if (d3dDLL == NULL) {
      EngineError("not find d3d8.dll");
  }
  Direct3DCreate8 = (pDirect3DCreate8)GetProcAddress(d3dDLL, "Direct3DCreate8");
  pD3D = Direct3DCreate8(D3D_SDK_VERSION);
  if (!pD3D) {
      return 0;
  }
  D3DDISPLAYMODE dmode;
  hr = pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &dmode);
  if (FAILED(hr)) {
      return 0;
  }
  ZeroMemory(&d3dpp, sizeof(d3dpp));
  d3dpp.Windowed = !fullScreen;
  if(fullScreen) {
      /* fullscreen mode */
      d3dpp.BackBufferWidth  = width;
      d3dpp.BackBufferHeight = height;
      d3dpp.BackBufferFormat = D3DFMT_R5G6B5;	//16bit
  } else {
      /* window mode */
      d3dpp.BackBufferWidth  = width;
      d3dpp.BackBufferHeight = height;
      d3dpp.BackBufferFormat = dmode.Format;
  }
  d3dpp.BackBufferCount = 1;
  d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
  
  /* create z buffer */
  d3dpp.EnableAutoDepthStencil = 1;
  d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

  /* hard ware hal */
  hr = pD3D->CreateDevice(0, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp,&pd3dDevice);
  if (SUCCEEDED(hr)) {
      WriteLog("hard ware hal mode\n");
      goto succeed;
  }
  /* soft ware hal */
  hr = pD3D->CreateDevice(0, D3DDEVTYPE_HAL, hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &pd3dDevice);
  if (SUCCEEDED(hr)) {
      WriteLog("soft ware hal mode\n");
      goto succeed;
  }
  /* REF */      
  hr = pD3D->CreateDevice(0, D3DDEVTYPE_REF, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, &pd3dDevice);
  if (SUCCEEDED(hr)) {
      WriteLog("ref mode\n");      
      goto succeed;
  }
  return 0;
  
succeed:
  /* get back buffer */
  pd3dDevice->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer);
  SetDefaultRenderState();
  WriteLog("init video\n");
  return 1;
}

static void QuitVideo();

int InitVideo(int fullScreen, int width, int height)
{
    AddFinalize(QuitVideo);
    return InitD3D(hWnd, fullScreen, width, height);
}    

static void QuitSurface();
static void QuitTexture();

static void QuitVideo()
{
    QuitTexture();
    QuitSurface();
    SAFE_RELEASE(pBackBuffer);
    SAFE_RELEASE(pd3dDevice);
    SAFE_RELEASE(pD3D);
    if (d3dDLL != NULL) {
        FreeLibrary(d3dDLL);
    }
    WriteLog("quit d3d\n");
}    

#define FVF (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1)

struct Vertex {
    float x, y, z, w;
    D3DCOLOR col;
    float tu, tv;
};

#define INIT_VERTEX(v, x0, y0, tu0, tv0, r, g, b, a)    \
    do {					\
	v.x = x0;				\
	v.y = y0;				\
	v.z = 0.0f;				\
	v.w = 1.0f;				\
	v.tu = tu0;				\
	v.tv = tv0;				\
	v.col = D3DCOLOR_RGBA(r, g, b, a);      \
    } while (0)

static struct Vertex vertex[4];

void _DrawVertex(float x0, float y0, float x1, float y1,
                 float tu0, float tv0, float tu1, float tv1,
                 byte r, byte g, byte b, byte a)
{
    INIT_VERTEX(vertex[0], x0, y0, tu0, tv0, r, g, b, a);
    INIT_VERTEX(vertex[1], x1, y0, tu1, tv0, r, g, b, a);
    INIT_VERTEX(vertex[2], x0, y1, tu0, tv1, r, g, b, a);
    INIT_VERTEX(vertex[3], x1, y1, tu1, tv1, r, g, b, a);
    pd3dDevice->SetVertexShader(FVF);          
    pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, vertex, sizeof(Vertex));
}    

struct RenderFrame {
    int optSize;
    int opt[10];
    Vertex vertex[4];
    LPDIRECT3DTEXTURE8 texture;
};

LPDIRECT3DTEXTURE8 CreateTexture(char *name)
{
    LPDIRECT3DTEXTURE8 texture = NULL;
    char *data;
    int size;

    if (texture != NULL) {
        WriteLog("read from file: %s\n", name);
        return texture;
    }
    data = LoadFile(name, &size);
    if (!data) {
        EngineError("not find %s\n", name);
    }
    D3DXCreateTextureFromFileInMemory(pd3dDevice, data, size, &texture);
    free(data);
    return texture;
}

static LPDIRECT3DTEXTURE8 texture[256];
static int textureSize = 0;

int ReadTexture(char *name)
{
    int i = textureSize;
    
    texture[textureSize++] = CreateTexture(name);
    return i;
}

static void QuitTexture()
{
    int i;
    for (i = 0; i < textureSize; i++) {
        SAFE_RELEASE(texture[i]);
    }
}    

LPDIRECT3DSURFACE8 CreateSurface(char *name)
{
    LPDIRECT3DSURFACE8 surface = NULL;
    D3DSURFACE_DESC desc;
    char *data;
    int size;

    pBackBuffer->GetDesc(&desc);
    pd3dDevice->CreateImageSurface(ENGINE_WIDTH, ENGINE_HEIGTH, desc.Format, &surface);
    data = LoadFile(name, &size);
    if (data == NULL) {
        EngineError("not find %s\n", name);
    }
    D3DXLoadSurfaceFromFileInMemory(surface, NULL, NULL, data, size,
                                    NULL, D3DX_DEFAULT, 0, NULL);
    free(data);
    return surface;
}

static LPDIRECT3DSURFACE8 surface[256];
static int surfaceSize = 0;

int ReadSurface(char *name)
{
    int i = surfaceSize;
    surface[surfaceSize++] = CreateSurface(name);
    return i;
}

void DrawTexture(int id)
{
    pd3dDevice->SetTexture(0, texture[id]);
}

void DrawTextureClear()
{
    pd3dDevice->SetTexture(0, NULL);
}

void DrawSurface(int id, int x0, int y0, int x1, int y1)
{
    RECT SrcRect;
    
    SetRect(&SrcRect, x0, y0, x1, y1);
    pd3dDevice->CopyRects(surface[id], &SrcRect, 1, pBackBuffer, NULL);
}

static void QuitSurface()
{
    int i;
    for (i = 0; i < surfaceSize; i++) {
        SAFE_RELEASE(surface[i]);
    }
}    

void DrawBackGround(byte r, byte g, byte b)
{    
    pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(r,g,b), 1.0f, 0);
}

int DrawEngine()
{
    HRESULT hr;
    
    hr = pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,0,150), 1.0f, 0);
    if (FAILED(hr)) {
        return 0;
    }
    pd3dDevice->BeginScene();
    if (DrawMain != NULL) {
        DrawMain();
    }
    pd3dDevice->EndScene();
    hr = pd3dDevice->Present(NULL, NULL, NULL, NULL);
    if (FAILED(hr)) {
        pd3dDevice->Reset(&d3dpp);
    }
    return 0;
}    



