#define DIRECTINPUT_VERSION 0x0800
#include <windows.h>
#include <dinput.h>
#include <stdio.h>
#include "lib.h"
#include "engine.h"

/* direct Input */
#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")

#define SAFE_RELEASE(p) {if(p != NULL) {p->Release(); p = NULL;}}

static LPDIRECTINPUT8 pDInput;
static LPDIRECTINPUTDEVICE8 pDIDevice;
static BYTE keyState[256];

/* joystick */
static char joystickOn = 0;
static LPDIRECTINPUTDEVICE8 pJoyStickDevice;
static DIDEVCAPS            diDevCaps;  

int InitJoyStick(HINSTANCE hInst, HWND hWnd);

struct InputConf {
    BYTE up;
    BYTE down;
    BYTE left;
    BYTE right;
    BYTE z;
    BYTE x;
    BYTE c;
    BYTE a;
    BYTE s;
    BYTE d;
    BYTE f;
};
static struct InputConf keybord = {
    DIK_UP,
    DIK_DOWN,
    DIK_LEFT,
    DIK_RIGHT,
    DIK_Z,
    DIK_X,
    DIK_C,
    DIK_A,
    DIK_S,
    DIK_D,
    DIK_F,
};

// DIJOFS_BUTTON2
static struct InputConf joystick = {
    NULL,
    NULL,
    NULL,
    NULL,
    0,
    1,
    2,
    3,
    4,
    5,
    6
};

static int InitDInput(HINSTANCE hInst, HWND hWnd)
{
  HRESULT hr;

  hr = DirectInput8Create(hInst, DIRECTINPUT_VERSION, IID_IDirectInput8,
                          (LPVOID*)&pDInput, NULL);
  if(FAILED(hr)) return 0;
  
  // デバイス・オブジェクトを作成
  hr = pDInput->CreateDevice(GUID_SysKeyboard, &pDIDevice, NULL); 
  if (FAILED(hr)) return 0;

  // データ形式を設定
  hr = pDIDevice->SetDataFormat(&c_dfDIKeyboard);
  if (FAILED(hr)) return 0;
  
  //モードを設定（フォアグラウンド＆非排他モード）
  hr = pDIDevice->SetCooperativeLevel(hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
  if (FAILED(hr)) return 0;

  DIPROPDWORD dipdw;
  ZeroMemory(&dipdw, sizeof(DIPROPDWORD));
  dipdw.diph.dwSize       = sizeof(DIPROPDWORD);
  dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
  dipdw.diph.dwObj        = 0;
  dipdw.diph.dwHow        = DIPH_DEVICE;
  dipdw.dwData            = 8; // buffsize
  hr = pDIDevice->SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph);
  if(FAILED(hr)) {
      return 0;
  }
  ZeroMemory(keyState, 256);
  if (InitJoyStick(hInst, hWnd)) {
      WriteLog("init joystick\n");
      joystickOn = 1;
  }
  WriteLog("init input\n");
  return 1;
}

extern HWND hWnd; 
extern HINSTANCE hInst;

static void QuitInput();

int InitInput()
{
    AddFinalize(QuitInput);
    return InitDInput(hInst, hWnd);
}    

static void QuitInput()
{
  if (pDIDevice != NULL) {
     pDIDevice->Unacquire();
  }
  SAFE_RELEASE(pDIDevice);
  if (joystickOn) {
      pJoyStickDevice->Unacquire();
      SAFE_RELEASE(pJoyStickDevice);
  }
  SAFE_RELEASE(pDInput);
  WriteLog("quit dinput\n");
}  

void StopInput(int fr)
{
  if (fr) {
    pDIDevice->Unacquire();
    if (joystickOn) {
        pJoyStickDevice->Unacquire();
    }
  } else {  
    pDIDevice->Acquire();
    if (joystickOn) {
        pJoyStickDevice->Acquire();
    }
  }  
}  

static void ReadJoyStick();

void ReadInput()
{
    HRESULT hr;
    
    if (joystickOn) {
        ReadJoyStick();
    }
    hr = pDIDevice->GetDeviceState(256, keyState);
    if SUCCEEDED( hr ) {
	    int i;
	    for (i = 0; i < 256; i++) {
		keyState[i] = (keyState[i] & 0x80);
	    }
	} else {
	pDIDevice->Acquire();
	ZeroMemory(keyState, 256);
    }
}

// ジョイスティックを列挙する関数
BOOL CALLBACK EnumJoysticksCallback(const DIDEVICEINSTANCE* lpddi, LPVOID lpContext)
{
    HRESULT hr;

    // 列挙されたジョイスティックへのインターフェイスを取得する。
    hr = pDInput->CreateDevice(lpddi->guidInstance, &pJoyStickDevice, NULL);
    if (FAILED(hr)) {
        return DIENUM_CONTINUE;
    }
    // ジョイスティックの能力を調べる
    diDevCaps.dwSize = sizeof(DIDEVCAPS);
    hr = pJoyStickDevice->GetCapabilities(&diDevCaps);
    if (FAILED(hr)) {
        return DIENUM_CONTINUE;
    }
    return DIENUM_STOP;
}

// ジョイスティックの軸を列挙する関数
BOOL CALLBACK EnumAxesCallback(LPCDIDEVICEOBJECTINSTANCE lpddoi, LPVOID lpvRef)
{
    HRESULT hr;
    DIPROPRANGE diprg;
    ZeroMemory(&diprg, sizeof(diprg));
    diprg.diph.dwSize       = sizeof(diprg); 
    diprg.diph.dwHeaderSize = sizeof(diprg.diph); 
    diprg.diph.dwObj        = lpddoi->dwType;
    diprg.diph.dwHow        = DIPH_BYID;
    diprg.lMin              = -1000;
    diprg.lMax              = +1000;
    hr = pJoyStickDevice->SetProperty(DIPROP_RANGE, &diprg.diph);
    if (FAILED(hr)) {
        return DIENUM_STOP;
    }
    return DIENUM_CONTINUE;
}

int InitJoyStick(HINSTANCE hInst, HWND hWnd)
{
    HRESULT hr;

    //ジョイスティックを探す
    hr = pDInput->EnumDevices(DI8DEVCLASS_GAMECTRL, EnumJoysticksCallback, NULL, DIEDFL_ATTACHEDONLY);
    if(pJoyStickDevice == NULL) {
        return 0;
    }
    //データフォーマットを設定
    hr = pJoyStickDevice->SetDataFormat(&c_dfDIJoystick);
    if(FAILED(hr)){
        return 0;
    }
    //モードを設定
    hr = pJoyStickDevice->SetCooperativeLevel(hWnd, DISCL_NONEXCLUSIVE  | DISCL_BACKGROUND);
    if(FAILED(hr)){
        return 0;
    }
    hr = pJoyStickDevice->EnumObjects(EnumAxesCallback, NULL, DIDFT_AXIS);
    if (FAILED(hr)) {
        return 0;
    }
    DIPROPDWORD diprop;
    ZeroMemory(&diprop, sizeof(DIPROPDWORD));
    diprop.diph.dwSize  = sizeof(diprop); 
    diprop.diph.dwHeaderSize = sizeof(diprop.diph); 
    diprop.diph.dwObj   = 0;
    diprop.diph.dwHow   = DIPH_DEVICE;
    diprop.dwData       = DIPROPAXISMODE_ABS;
    pJoyStickDevice->SetProperty(DIPROP_AXISMODE, &diprop.diph);
    pJoyStickDevice->Acquire();
    return 1;
}

static DIJOYSTATE dijs;

static void ReadJoyStick()
{
    pJoyStickDevice->Poll();
    pJoyStickDevice->GetDeviceState(sizeof(DIJOYSTATE), &dijs);
}

static int GetJoyStick(struct InputConf *joystick, enum InputKey key) 
{
    switch (key) {
    case KEY_UP:
        return (dijs.lY > +1000 / 2) ? 1 : 0;
        break;

    case KEY_DOWN:
        return (dijs.lY < -1000 / 2) ? 1 : 0;        
        break;

    case KEY_LEFT:
        return (dijs.lX < -1000 / 2) ? 1 : 0;
        break;

    case KEY_RIGHT:
        return (dijs.lX > +1000 / 2) ? 1 : 0;
        break;

    case KEY_Z:
        return (dijs.rgbButtons[joystick->z] & 0x80) ? 1 : 0;                            
        break;

    case KEY_X:
        return (dijs.rgbButtons[joystick->x] & 0x80) ? 1 : 0;                            
        break;

    case KEY_C:
        return (dijs.rgbButtons[joystick->c] & 0x80) ? 1 : 0;                            
        break;

    case KEY_A:
        return (dijs.rgbButtons[joystick->a] & 0x80) ? 1 : 0;                                    
        break;

    case KEY_S:
        return (dijs.rgbButtons[joystick->s] & 0x80) ? 1 : 0;                                    
        break;

    case KEY_D:
        return (dijs.rgbButtons[joystick->d] & 0x80) ? 1 : 0;                            
        break;

    case KEY_F:
        return (dijs.rgbButtons[joystick->f] & 0x80) ? 1 : 0;                            
        break;
    }
    return 0;
}    

static int GetKeyBord(struct InputConf *keybord, enum InputKey key)
{
    BYTE type;
    
    switch (key) {
    case KEY_UP:
        type = keybord->up;
        break;

    case KEY_DOWN:
        type = keybord->down;
        break;

    case KEY_LEFT:
        type = keybord->left;
        break;

    case KEY_RIGHT:
        type = keybord->right;
        break;

    case KEY_Z:
        type = keybord->z;
        break;

    case KEY_X:
        type = keybord->x;
        break;

    case KEY_C:
        type = keybord->c;
        break;

    case KEY_A:
        type = keybord->a;
        break;

    case KEY_S:
        type = keybord->s;
        break;
        
    case KEY_D:
        type = keybord->d;
        break;

    case KEY_F:
        type = keybord->f;
        break;
    }
    return (keyState[type] & 0x80) ? 1 : 0;                    
}    

int PushKey(enum InputKey key)
{
    if (joystickOn) {
        if (GetJoyStick(&joystick, key)) {
            return 1;
        } 
    }
    return GetKeyBord(&keybord, key) ? 1 : 0;
}    

