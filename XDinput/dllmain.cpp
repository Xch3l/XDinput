#include "stdafx.h"
#include <dinput.h>
#include <xinput.h>

#include "dll.h"
#include "xdinput.h"
#include "DeviceStatus.h"

///////////////////////////////////////////////////////////////////////

typedef HRESULT (WINAPI* dinputDirectInputCreate)(HINSTANCE hInst, DWORD dwVersion, REFIID riidGUID, LPVOID* ppvOut, LPUNKNOWN punkOuter);

static dinputDirectInputCreate directInputCreate;
static HMODULE hDinputDll;
static LPVOID lpdi, xdinput;
static DeviceStatus* hDevStat;

///////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
#include <crtdbg.h>
#include <io.h>
#include <fcntl.h>

void OpenConsole() {
  _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

  AllocConsole();

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
}
#else
#define OpenConsole() /* DUMMY */
#endif

///////////////////////////////////////////////////////////////////////

HRESULT WINAPI DirectInput8Create(HINSTANCE hInst, DWORD dwVersion, REFIID riidGUID, LPVOID* ppvOut, LPUNKNOWN punkOuter) {
  #pragma C_EXPORT
  BOOL isAnsi;

  if(dwVersion < 0x800)
    return DIERR_OLDDIRECTINPUTVERSION;

  printf("Creating DirectInput8 instance\n");
  printf("  Version: %04X\n", dwVersion);

  if(!(hDinputDll = LoadLibrary(TEXT("C:\\WINDOWS\\system32\\dinput8.dll"))))
    return E_FAIL;

  if(!(directInputCreate = (dinputDirectInputCreate) GetProcAddress(hDinputDll, "DirectInput8Create")))
    return E_FAIL;

  // Create a base interface to work on when not referring to our wrapper
  if(FAILED(directInputCreate(hInst, dwVersion, riidGUID, (VOID**) &lpdi, punkOuter)))
    return E_FAIL;

  if(isAnsi = (riidGUID == IID_IDirectInput8A))
    printf("  ANSI Mode\n");

  xdinput = *ppvOut = new XDinput(isAnsi, (LPDIRECTINPUT8) lpdi);
  return DI_OK;
}

HRESULT WINAPI DirectInputCreateEx(HINSTANCE hInst, DWORD dwVersion, REFIID riidGUID, LPVOID* ppvOut, LPUNKNOWN pUnkOuter) {
  #pragma C_EXPORT
  BOOL isAnsi = FALSE;

  printf("Creating DirectInput instance\n");
  printf("  Version: %04X\n", dwVersion);

  if(dwVersion == 0x0700) {
    if(!(hDinputDll = LoadLibrary(TEXT("C:\\WINDOWS\\system32\\dinput.dll"))))
      return E_FAIL;

    if(!(directInputCreate = (dinputDirectInputCreate) GetProcAddress(hDinputDll, "DirectInputCreateEx")))
      return E_FAIL;

    // Create a base interface to work on when not referring to our wrapper
    if(FAILED(directInputCreate(hInst, dwVersion, riidGUID, (VOID**) &lpdi, pUnkOuter)))
      return E_FAIL;

    if(isAnsi = (riidGUID == IID_IDirectInput7A))
      printf("  ANSI mode\n");

    xdinput = *ppvOut = new XDinput7(isAnsi, (LPDIRECTINPUT7) lpdi);
    return DI_OK;
  }

  return DIERR_OLDDIRECTINPUTVERSION;
}

///////////////////////////////////////////////////////////////////////

// Function export for rundll32
//    Run as: "rundll32.exe XDinput.dll,TestControllers"
void CALLBACK TestControllersA(HWND hWnd, HINSTANCE hInst, LPTSTR lpCmdLine, int nCmdShow) {
  #pragma C_EXPORT
  MessageBox(0, L"Click OK when done", L"XDinput", MB_OK);
}

BOOL APIENTRY DllMain(HMODULE hInst, DWORD dwReason, LPVOID lpReserved) {
  switch(dwReason) {
    case DLL_PROCESS_ATTACH:
      OpenConsole();
      hDevStat = new DeviceStatus(hInst);
      break;

    case DLL_PROCESS_DETACH:
      if(lpdi)
        ((LPUNKNOWN) lpdi)->Release();

      if(xdinput) {
        ((LPUNKNOWN) xdinput)->Release();
        delete xdinput;
      }

      if(hDinputDll) {
        FreeLibrary(hDinputDll);
        delete hDinputDll;
      }

      delete hDevStat;
      break;

    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
      break;
  }

  return TRUE;
}

