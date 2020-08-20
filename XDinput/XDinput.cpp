#include "stdafx.h"
#include <xinput.h>
#include "XDinput.h"
#include "JoyConfig.h"
#include "XDInputDevice.h"

// Constructor
XDinput::XDinput() {}

XDinput::XDinput(int isAnsi, LPDIRECTINPUT8 lpdi) {
  controllerCount = 0;
  IsAnsi = isAnsi;
  diBase = lpdi;
}

// Destructor
XDinput::~XDinput() {
}

///////////////////////////////////////////////////
/// IUnknown implements
ULONG WINAPI XDinput::AddRef() {
  if(controllerCount == 4)
    return 4;

  if(controllerCount == 0)
    XInputEnable(TRUE);

  return controllerCount++;
}

HRESULT WINAPI XDinput::QueryInterface(REFIID riid, LPVOID *ppvObject) {
  if(controllerCount >= 4)
    return E_NOINTERFACE;

  int index = AddRef();

  if(riid == IID_IDirectInputJoyConfig8)
    *ppvObject = new JoyConfig(index, IsAnsi);
  else
  if(riid == IID_IDirectInput)
    *ppvObject = new JoyConfigLegacy(index, IsAnsi);

  return DI_OK;
}

ULONG WINAPI XDinput::Release() {
  if(--controllerCount <= 0) {
    XInputEnable(FALSE);
    delete this;
    return 0;
  }

  return controllerCount;
}

///////////////////////////////////////////////////
/// IDirectInput8 implements
HRESULT WINAPI XDinput::ConfigureDevices(LPDICONFIGUREDEVICESCALLBACK lpdiCallback, LPDICONFIGUREDEVICESPARAMS lpdiCDParams, DWORD dwFlags, LPVOID pvRefData) {
  return (HRESULT) 0x8007000EL;
}

HRESULT WINAPI XDinput::CreateDevice(REFGUID rGuid, LPDIRECTINPUTDEVICE8 *lplpDirectInputDevice, LPUNKNOWN pUnkOuter) {
  // Our special GUID
  if(XDINPUT_IS_CONTROLLER_GUID(rGuid)) {
    int index = XDINPUT_GETCONTROLLER(rGuid);

    if(index > 4)
      return DIERR_INVALIDPARAM;

    AddRef();
    *lplpDirectInputDevice = new XDinputDevice(index, IsAnsi);
    return DI_OK;
  }

  return diBase->CreateDevice(rGuid, lplpDirectInputDevice, pUnkOuter);
}

HRESULT WINAPI XDinput::EnumDevices(DWORD dwDevType, LPDIENUMDEVICESCALLBACK lpCallback, LPVOID pvRef, DWORD dwFlags) {
  // call DirectInput when requested devices other than gamepads
  if(dwDevType != DI8DEVCLASS_GAMECTRL)
    return diBase->EnumDevices(dwDevType, lpCallback, pvRef, dwFlags);

  XINPUT_STATE state;

  for(int i = 0; i < 4; i++) {
    ZeroMemory(&state, sizeof(XINPUT_STATE));

    if(XInputGetState(i, &state) == ERROR_DEVICE_NOT_CONNECTED && (dwFlags & DIEDBSFL_ATTACHEDONLY))
      continue;

    if(IsAnsi) {
      DIDEVICEINSTANCEA ddi;
      ddi.dwSize = sizeof(DIDEVICEINSTANCE);
      XDINPUT_CONTROLLER_GUID(ddi.guidInstance, i);
      ddi.guidProduct = XDINPUT_PRODUCT_GUID;
      ddi.dwDevType = DI8DEVTYPE_GAMEPAD;
      ddi.guidFFDriver = XDINPUT_VIBRATOR_GUID;
      ddi.wUsage = -1;

      sprintf_s(ddi.tszInstanceName, MAX_PATH, "XDINPUT #%d", i + 1);
      sprintf_s(ddi.tszProductName, MAX_PATH, "XDINPUT Wrapper");

      if(((LPDIENUMDEVICESCALLBACKA) lpCallback)(&ddi, pvRef) == DIENUM_STOP)
        return DI_OK;

      continue;
    }

    DIDEVICEINSTANCEW ddi;
    ddi.dwSize = sizeof(DIDEVICEINSTANCE);
    XDINPUT_CONTROLLER_GUID(ddi.guidInstance, i);
    ddi.guidProduct = XDINPUT_PRODUCT_GUID;
    ddi.dwDevType = DI8DEVTYPE_GAMEPAD;
    ddi.guidFFDriver = XDINPUT_VIBRATOR_GUID;
    ddi.wUsage = -1;

    swprintf_s(ddi.tszInstanceName, MAX_PATH, L"XDINPUT #%d", i + 1);
    swprintf_s(ddi.tszProductName, MAX_PATH, L"XDINPUT Wrapper");

    if(lpCallback(&ddi, pvRef) == DIENUM_STOP)
      return DI_OK;
  }

  return DIERR_NOMOREITEMS;
}

HRESULT WINAPI XDinput::EnumDevicesBySemantics(LPCWSTR ptszUserName, LPDIACTIONFORMAT lpdiActionFormat, LPDIENUMDEVICESBYSEMANTICSCB lpCallback, LPVOID pvRef, DWORD dwFlags) {
  return (HRESULT) 0x8007000EL;
}

HRESULT WINAPI XDinput::FindDevice(REFGUID rGuidClass, LPCTSTR ptszName, LPGUID pGuidInstance) {
  return (HRESULT) 0x8007000EL;
}

HRESULT WINAPI XDinput::GetDeviceStatus(REFGUID rGuidInstance) {
  return (HRESULT) 0x8007000EL;
}

HRESULT WINAPI XDinput::Initialize(HINSTANCE hInst, DWORD dwVersion) {
  return (HRESULT) 0x8007000EL;
}

HRESULT WINAPI XDinput::RunControlPanel(HWND hwndOwner, DWORD dwFlags) {
  return (HRESULT) 0x8007000EL;
}

///////////////////////////////////////////////////

// Constructor
XDinput7::XDinput7(int isAnsi, LPDIRECTINPUT7 lpdi) : XDinput(isAnsi, (LPDIRECTINPUT8) lpdi) {}

// Destructor
XDinput7::~XDinput7() {}

///////////////////////////////////////////////////
/// IUnknown implements
ULONG WINAPI XDinput7::AddRef() {
  return XDinput::AddRef();
}

HRESULT WINAPI XDinput7::QueryInterface(REFIID riid, LPVOID *ppvObject) {
  return XDinput::QueryInterface(riid, ppvObject);
}

ULONG WINAPI XDinput7::Release() {
  return XDinput::Release();
}

///////////////////////////////////////////////////
/// IDirectInput7 implements
HRESULT WINAPI XDinput7::CreateDevice(REFGUID rGuid, LPDIRECTINPUTDEVICE *lplpDirectInputDevice, LPUNKNOWN pUnkOuter) {
  return XDinput::CreateDevice(rGuid, (LPDIRECTINPUTDEVICE8*) lplpDirectInputDevice, pUnkOuter);
}

HRESULT WINAPI XDinput7::EnumDevices(DWORD dwDevType, LPDIENUMDEVICESCALLBACK lpCallback, LPVOID pvRef, DWORD dwFlags) {
  return XDinput::EnumDevices(dwDevType, lpCallback, pvRef, dwFlags);
}

HRESULT WINAPI XDinput7::FindDevice(REFGUID rGuidClass, LPCTSTR ptszName, LPGUID pGuidInstance) {
  return XDinput::FindDevice(rGuidClass, ptszName, pGuidInstance);
}

HRESULT WINAPI XDinput7::GetDeviceStatus(REFGUID rGuidInstance) {
  return XDinput::GetDeviceStatus(rGuidInstance);
}

HRESULT WINAPI XDinput7::Initialize(HINSTANCE hInst, DWORD dwVersion) {
  return XDinput::Initialize(hInst, dwVersion);
}

HRESULT WINAPI XDinput7::RunControlPanel(HWND hwndOwner, DWORD dwFlags) {
  return XDinput::RunControlPanel(hwndOwner, dwFlags);
}

HRESULT WINAPI XDinput7::CreateDeviceEx(REFGUID rGuid, REFIID riid, LPVOID *lplpObject, LPUNKNOWN pUnkOuter) {
  return XDinput::CreateDevice(rGuid, (LPDIRECTINPUTDEVICE8*) lplpObject, pUnkOuter);
}