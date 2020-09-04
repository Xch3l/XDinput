#include "stdafx.h"
#include <dinput.h>

#include "dll.h"
#include "XDinput.h"
#include "XDinput7.h"
#include "XDinput2.h"
#include "XDinput1.h"
#include "DeviceStatus.h"

///////////////////////////////////////////////////////////////////////

typedef HRESULT (WINAPI* dinputDirectInputCreateEx)(HINSTANCE hInst, DWORD dwVersion, REFIID riidGUID, LPVOID* ppvOut, LPUNKNOWN punkOuter);
static dinputDirectInputCreateEx directInputCreateEx;

typedef HRESULT (WINAPI* dinputDirectInputCreate)(HINSTANCE hInst, DWORD dwVersion, LPVOID* ppvOut, LPUNKNOWN punkOuter);
static dinputDirectInputCreate directInputCreate;

static HMODULE hDinputDll = NULL;
static LPVOID lpdi, xdinput = NULL;
static DeviceStatus* hDevStat = NULL;
DWORD DinputVersion;

///////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
#include <crtdbg.h>
#include <io.h>
#include <fcntl.h>

void OpenConsole() {
  _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

  AllocConsole();
  SetConsoleTitleW(L"XDinput Console");
  Sleep(40);

  LONG s = (LONG) GetStdHandle(STD_OUTPUT_HANDLE);
  INT h = _open_osfhandle(s, _O_TEXT);
  FILE* f = _fdopen(h, "w");
  *stdout = *f;
  setvbuf(stdout, NULL, _IONBF, 0);

  s = (LONG) GetStdHandle(STD_INPUT_HANDLE);
  h = _open_osfhandle(s, _O_TEXT);
  f = _fdopen(h, "r");
  *stdin = *f;
  setvbuf(stdin, NULL, _IONBF, 0);

  s = (LONG) GetStdHandle(STD_ERROR_HANDLE);
  h = _open_osfhandle(s, _O_TEXT);
  f = _fdopen(h, "w");
  *stderr = *f;
  setvbuf(stderr, NULL, _IONBF, 0);

  // Move console window below Status window
  HWND hCon = FindWindow(NULL, L"XDinput Console");

  RECT rd, rc;
  GetWindowRect(hCon, &rc);
  GetWindowRect(hDevStat->hWnd, &rd);
  MoveWindow(hCon, rd.right + 5, rd.top, rc.right - rc.left, rc.bottom - rc.top, TRUE);
}
#else
#define OpenConsole() /* DUMMY */
#endif

///////////////////////////////////////////////////////////////////////

HRESULT WINAPI DirectInput8Create(HINSTANCE, DWORD, REFIID, LPVOID*, LPUNKNOWN);
HRESULT WINAPI DirectInputCreateA(HINSTANCE, DWORD, REFIID, LPVOID*, LPUNKNOWN);
HRESULT WINAPI DirectInputCreateW(HINSTANCE, DWORD, REFIID, LPVOID*, LPUNKNOWN);
HRESULT WINAPI DirectInputCreateEx(HINSTANCE, DWORD, REFIID, LPVOID*, LPUNKNOWN);

///////////////////////////////////////////////////////////////////////

HRESULT WINAPI DirectInput8Create(HINSTANCE hInst, DWORD dwVersion, REFIID riidGUID, LPVOID* ppvOut, LPUNKNOWN punkOuter) {
#pragma C_EXPORT
  BOOL isAnsi = (riidGUID == IID_IDirectInput8A);
  DinputVersion = dwVersion;
  HRESULT hr;

  if(dwVersion < 0x0800)
    return DIERR_OLDDIRECTINPUTVERSION;

#ifdef _DEBUG
  printf("Creating DirectInput8%c instance\n", isAnsi ? 'A' : 'W');
  printf("  Version: %d.%02d\n", (dwVersion & 0xFF00) >> 8, dwVersion & 255);
#endif

  if(!(hDinputDll = LoadLibrary(TEXT("C:\\WINDOWS\\system32\\dinput8.dll"))))
    return E_FAIL;

  if(!(directInputCreateEx = (dinputDirectInputCreateEx) GetProcAddress(hDinputDll, "DirectInput8Create")))
    return E_FAIL;

  // Create a base interface to work on when not referring to our wrapper
  if(FAILED(hr = directInputCreateEx(hInst, dwVersion, riidGUID, &lpdi, punkOuter)))
    return hr;

  xdinput = *ppvOut = new XDinput(isAnsi, lpdi);
  return DI_OK;
}

HRESULT WINAPI DirectInputCreateA(HINSTANCE hInst, DWORD dwVersion, LPVOID* ppvOut, LPUNKNOWN pUnkOuter) {
#pragma C_EXPORT
  DinputVersion = dwVersion;
  HRESULT hr;

#ifdef _DEBUG
  puts("Creating DirectInputA instance");
  printf("  Version: %d.%02d\n", (dwVersion & 0xFF00) >> 8, dwVersion & 255);
#endif

  if(!(hDinputDll = LoadLibrary(TEXT("C:\\WINDOWS\\system32\\dinput.dll"))))
    return E_FAIL;

  if(!(directInputCreate = (dinputDirectInputCreate) GetProcAddress(hDinputDll, "DirectInputCreateA")))
    return E_FAIL;

  // Create a base interface to work on when not referring to our wrapper
  if(FAILED(hr = directInputCreate(hInst, dwVersion, &lpdi, pUnkOuter)))
    return hr;

  if(dwVersion < 0x0200) { // DirectInput
    xdinput = *ppvOut = new XDinput1(TRUE, lpdi);
    return DI_OK;
  } else
  if(dwVersion < 0x0700) { // DirectInput2
    xdinput = *ppvOut = new XDinput2(TRUE, lpdi);
    return DI_OK;
  } else
  if(dwVersion == 0x0700) { // DirectInput7
    xdinput = *ppvOut = new XDinput7(TRUE, lpdi);
    return DI_OK;
  }

  return DIERR_INVALIDPARAM;
}

HRESULT WINAPI DirectInputCreateW(HINSTANCE hInst, DWORD dwVersion, LPVOID* ppvOut, LPUNKNOWN pUnkOuter) {
#pragma C_EXPORT
  DinputVersion = dwVersion;
  HRESULT hr;

#ifdef _DEBUG
  puts("Creating DirectInputW instance");
  printf("  Version: %d.%02d\n", (dwVersion & 0xFF00) >> 8, dwVersion & 255);
#endif

  if(!(hDinputDll = LoadLibrary(TEXT("C:\\WINDOWS\\system32\\dinput.dll"))))
    return E_FAIL;

  if(!(directInputCreate = (dinputDirectInputCreate) GetProcAddress(hDinputDll, "DirectInputCreateW")))
    return E_FAIL;

  // Create a base interface to work on when not referring to our wrapper
  if(FAILED(hr = directInputCreate(hInst, dwVersion, &lpdi, pUnkOuter)))
    return hr;

  if(dwVersion < 0x0200) { // DirectInput
    xdinput = *ppvOut = new XDinput1(FALSE, lpdi);
    return DI_OK;
  } else
  if(dwVersion < 0x0700) { // DirectInput2
    xdinput = *ppvOut = new XDinput2(FALSE, lpdi);
    return DI_OK;
  } else
  if(dwVersion == 0x0700) { // DirectInput7
    xdinput = *ppvOut = new XDinput7(FALSE, lpdi);
    return DI_OK;
  }

  return DIERR_INVALIDPARAM;
}

HRESULT WINAPI DirectInputCreateEx(HINSTANCE hInst, DWORD dwVersion, REFIID riidGUID, LPVOID* ppvOut, LPUNKNOWN pUnkOuter) {
#pragma C_EXPORT
  BOOL isAnsi = (riidGUID == IID_IDirectInput7A || riidGUID == IID_IDirectInput2A || riidGUID == IID_IDirectInputA);
  DinputVersion = dwVersion;
  HRESULT hr;

#ifdef _DEBUG
  printf("Creating DirectInput%c instance", isAnsi ? 'A' : 'W');
  printf("  Version: %d.%02d\n", (dwVersion & 0xFF00) >> 8, dwVersion & 255);
  printf("  GUID: {%08X-%04X-%04X-", riidGUID.Data1, riidGUID.Data2, riidGUID.Data3);

  for(int i = 0; i < 8; i++) {
    printf("%02X", riidGUID.Data4[i]);
    if(i == 1) printf("-");
  }

  printf("}");
  
  if(riidGUID == IID_IDirectInputA) puts(" IID_IDirectInputA"); else
  if(riidGUID == IID_IDirectInputW) puts(" IID_IDirectInputW"); else
  if(riidGUID == IID_IDirectInput2A) puts(" IID_IDirectInput2A"); else
  if(riidGUID == IID_IDirectInput2W) puts(" IID_IDirectInput2W"); else
  if(riidGUID == IID_IDirectInput7A) puts(" IID_IDirectInput7A"); else
  if(riidGUID == IID_IDirectInput7W) puts(" IID_IDirectInput7W");
#endif

  if(!(hDinputDll = LoadLibrary(TEXT("C:\\WINDOWS\\system32\\dinput.dll"))))
    return E_FAIL;

  if(!(directInputCreateEx = (dinputDirectInputCreateEx) GetProcAddress(hDinputDll, "DirectInputCreateEx")))
    return E_FAIL;

  // Create a base interface to work on when not referring to our wrapper
  if(FAILED(hr = directInputCreateEx(hInst, dwVersion, riidGUID, &lpdi, pUnkOuter)))
    return hr;

  if(dwVersion < 0x0200) { // DirectInput
    xdinput = *ppvOut = new XDinput1(isAnsi, lpdi);
    return DI_OK;
  } else
  if(dwVersion < 0x0700) { // DirectInput2
    xdinput = *ppvOut = new XDinput2(isAnsi, lpdi);
    return DI_OK;
  } else
  if(dwVersion == 0x0700) { // DirectInput7
    xdinput = *ppvOut = new XDinput7(isAnsi, lpdi);
    return DI_OK;
  }

  return DIERR_OLDDIRECTINPUTVERSION;
}

STDAPI DllCanUnloadNow(void) {
#pragma comment(linker, "/EXPORT:DllCanUnloadNow@=DllCanUnloadNow,PRIVATE")

  if(xdinput && ((XDinput*) xdinput)->controllerCount > 0)
    return S_FALSE;

  return S_OK;
}

///////////////////////////////////////////////////////////////////////

BOOL APIENTRY DllMain(HMODULE hInst, DWORD dwReason, LPVOID lpReserved) {
  switch(dwReason) {
    case DLL_PROCESS_ATTACH:
      hDevStat = new DeviceStatus(hInst);
      OpenConsole();
      XInputEnable(TRUE);
      return TRUE;

    case DLL_PROCESS_DETACH: {
      XInputEnable(FALSE);

      if(lpdi)
        ((LPUNKNOWN) lpdi)->Release();

      if(xdinput && (int) *(int*) ((LPUNKNOWN) xdinput) > 0) {
        ((LPUNKNOWN) xdinput)->Release();
        delete xdinput;
      }

      if(hDinputDll)
        FreeLibrary(hDinputDll);

      if(hDevStat > 0)
        delete hDevStat;

      return TRUE;
    }

    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
      return TRUE;
  }

  return FALSE;
}

