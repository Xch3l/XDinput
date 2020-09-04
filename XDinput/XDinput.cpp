#include "stdafx.h"
#include "XDinput.h"
#include "JoyConfig.h"
#include "XDInputDevice.h"
#include "XDinputDeviceLegacy.h"

// Constructor
XDinput::XDinput() {}

XDinput::XDinput(int isAnsi, LPVOID lpdi) {
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
  printf("[XDinput] AddRef\n");
  if(controllerCount < 4)
    controllerCount++;
  return controllerCount;
}

HRESULT WINAPI XDinput::QueryInterface(REFIID riid, LPVOID *ppvObject) {
  printf("[XDinput] QueryInterface\n");
  if(controllerCount >= 4)
    return E_NOINTERFACE;

  int index = AddRef();

  if(riid == IID_IDirectInputJoyConfig8) {
    *ppvObject = new JoyConfig(index, IsAnsi);
    return DI_OK;
  }

  if(riid == IID_IDirectInputA || riid == IID_IDirectInputW) {
    *ppvObject = new JoyConfigLegacy(index, IsAnsi);
    return DI_OK;
  }

  return E_NOINTERFACE;
}

ULONG WINAPI XDinput::Release() {
  printf("[XDinput] Release\n");
  if(controllerCount > 0)
    controllerCount--;

  if(controllerCount == 0) {
    delete this;
    return 0;
  }

  return controllerCount;
}

///////////////////////////////////////////////////
/// IDirectInput8 implements
HRESULT WINAPI XDinput::ConfigureDevices(LPDICONFIGUREDEVICESCALLBACK lpdiCallback, LPDICONFIGUREDEVICESPARAMS lpdiCDParams, DWORD dwFlags, LPVOID pvRefData) {
  printf("[XDinput] ConfigureDevices\n");
  return (HRESULT) 0x8007000EL;
}

HRESULT WINAPI XDinput::CreateDevice(REFGUID rGuid, LPDIRECTINPUTDEVICE8 *lplpDirectInputDevice, LPUNKNOWN pUnkOuter) {
  // Our special GUID
  if(XDINPUT_IS_CONTROLLER_GUID(rGuid)) {
    int index = XDINPUT_GETCONTROLLER(rGuid);
    printf("[XDinput] CreateDevice %d\n", index);

    if(index > 4)
      return DIERR_INVALIDPARAM;

    AddRef();

    if(DinputVersion >= 0x0800)
      *lplpDirectInputDevice = new XDinputDevice(index, IsAnsi);
    else
      *((LPDIRECTINPUTDEVICE*) lplpDirectInputDevice) = new XDinputDeviceLegacy(index, IsAnsi);

    return DI_OK;
  }

  if(IsAnsi) {
    if(DinputVersion < 0x0200)
      return ((LPDIRECTINPUTA) diBase)->CreateDevice(rGuid, (LPDIRECTINPUTDEVICEA*) lplpDirectInputDevice, pUnkOuter);
    if(DinputVersion < 0x0700)
      return ((LPDIRECTINPUT2A) diBase)->CreateDevice(rGuid, (LPDIRECTINPUTDEVICEA*) lplpDirectInputDevice, pUnkOuter);
    if(DinputVersion < 0x0800)
      return ((LPDIRECTINPUT7A) diBase)->CreateDevice(rGuid, (LPDIRECTINPUTDEVICEA*) lplpDirectInputDevice, pUnkOuter);
    return ((LPDIRECTINPUT8A) diBase)->CreateDevice(rGuid, (LPDIRECTINPUTDEVICE8A*) lplpDirectInputDevice, pUnkOuter);
  }

  if(DinputVersion < 0x0200)
    return ((LPDIRECTINPUTW) diBase)->CreateDevice(rGuid, (LPDIRECTINPUTDEVICEW*) lplpDirectInputDevice, pUnkOuter);
  if(DinputVersion < 0x0700)
    return ((LPDIRECTINPUT2W) diBase)->CreateDevice(rGuid, (LPDIRECTINPUTDEVICEW*) lplpDirectInputDevice, pUnkOuter);
  if(DinputVersion < 0x0800)
    return ((LPDIRECTINPUT7W) diBase)->CreateDevice(rGuid, (LPDIRECTINPUTDEVICEW*) lplpDirectInputDevice, pUnkOuter);
  return ((LPDIRECTINPUT8W) diBase)->CreateDevice(rGuid, (LPDIRECTINPUTDEVICE8W*) lplpDirectInputDevice, pUnkOuter);
}

HRESULT WINAPI XDinput::EnumDevices(DWORD dwDevType, LPDIENUMDEVICESCALLBACKW lpCallback, LPVOID pvRef, DWORD dwFlags) {
  if(dwDevType == DI8DEVCLASS_GAMECTRL || dwDevType == DI8DEVCLASS_ALL) {
    XINPUT_STATE state;
    printf("[XDinput] EnumDevices\n");

    for(int i = 0; i < 4; i++) {
      ZeroMemory(&state, sizeof(XINPUT_STATE));

      if(XInputGetState(i, &state) == ERROR_DEVICE_NOT_CONNECTED && dwFlags == DIEDBSFL_ATTACHEDONLY)
        continue;

      if(IsAnsi) {
        DIDEVICEINSTANCEA ddi;
        ddi.dwSize = sizeof(DIDEVICEINSTANCEA);
        XDINPUT_CONTROLLER_GUID(ddi.guidInstance, i);
        ddi.guidProduct = XDINPUT_PRODUCT_GUID;
        ddi.dwDevType = DIDEVTYPE_HID | DI8DEVCLASS_GAMECTRL;
        if(DinputVersion <= 0x0700) ddi.dwDevType |= DI8DEVTYPE_GAMEPAD;
        sprintf_s(ddi.tszInstanceName, MAX_PATH, XDINPUT_INSTANCENAMEA, i + 1);
        sprintf_s(ddi.tszProductName, MAX_PATH, XDINPUT_PRODUCTNAMEA);

        if(DinputVersion >= 0x0500) {
          ddi.guidFFDriver = XDINPUT_VIBRATOR_GUID;
          ddi.wUsage = ddi.wUsagePage = 0;
        }

        if(((LPDIENUMDEVICESCALLBACKA) lpCallback)(&ddi, pvRef) == DIENUM_STOP)
          break;

        continue;
      }

      DIDEVICEINSTANCEW ddi;
      ddi.dwSize = sizeof(DIDEVICEINSTANCEA);
      XDINPUT_CONTROLLER_GUID(ddi.guidInstance, i);
      ddi.guidProduct = XDINPUT_PRODUCT_GUID;
      ddi.dwDevType = DIDEVTYPE_HID | DI8DEVCLASS_GAMECTRL;
      if(DinputVersion <= 0x0700) ddi.dwDevType |= DI8DEVTYPE_GAMEPAD;
      swprintf_s(ddi.tszInstanceName, MAX_PATH, XDINPUT_INSTANCENAMEW, i + 1);
      swprintf_s(ddi.tszProductName, MAX_PATH, XDINPUT_PRODUCTNAMEW);

      if(DinputVersion >= 0x0500) {
        ddi.guidFFDriver = XDINPUT_VIBRATOR_GUID;
        ddi.wUsage = ddi.wUsagePage = 0;
      }

      if(lpCallback(&ddi, pvRef) == DIENUM_STOP)
        break;
    }
  }

  // Allow DirectInput to enumerate other devices
  if(IsAnsi) {
    if(DinputVersion < 0x0200)
      return ((LPDIRECTINPUTA) diBase)->EnumDevices(dwDevType, (LPDIENUMDEVICESCALLBACKA) lpCallback, pvRef, dwFlags);
    if(DinputVersion < 0x0700)
      return ((LPDIRECTINPUT2A) diBase)->EnumDevices(dwDevType, (LPDIENUMDEVICESCALLBACKA) lpCallback, pvRef, dwFlags);
    if(DinputVersion < 0x0800)
      return ((LPDIRECTINPUT7A) diBase)->EnumDevices(dwDevType, (LPDIENUMDEVICESCALLBACKA) lpCallback, pvRef, dwFlags);

    return ((LPDIRECTINPUT8A) diBase)->EnumDevices(dwDevType, (LPDIENUMDEVICESCALLBACKA) lpCallback, pvRef, dwFlags);
  }

  if(DinputVersion < 0x0200)
    return ((LPDIRECTINPUTW) diBase)->EnumDevices(dwDevType, (LPDIENUMDEVICESCALLBACKW) lpCallback, pvRef, dwFlags);
  if(DinputVersion < 0x0700)
    return ((LPDIRECTINPUT2W) diBase)->EnumDevices(dwDevType, (LPDIENUMDEVICESCALLBACKW) lpCallback, pvRef, dwFlags);
  if(DinputVersion < 0x0800)
    return ((LPDIRECTINPUT7W) diBase)->EnumDevices(dwDevType, (LPDIENUMDEVICESCALLBACKW) lpCallback, pvRef, dwFlags);

  return ((LPDIRECTINPUT8W) diBase)->EnumDevices(dwDevType, (LPDIENUMDEVICESCALLBACKW) lpCallback, pvRef, dwFlags);
}

HRESULT WINAPI XDinput::EnumDevicesBySemantics(LPCWSTR ptszUserName, LPDIACTIONFORMAT lpdiActionFormat, LPDIENUMDEVICESBYSEMANTICSCB lpCallback, LPVOID pvRef, DWORD dwFlags) {
  printf("[XDinput] EnumDevicesBySemantics\n");
  return (HRESULT) 0x8007000EL;
}

HRESULT WINAPI XDinput::FindDevice(REFGUID rGuidClass, LPCTSTR ptszName, LPGUID pGuidInstance) {
  printf("[XDinput] FindDevice\n");
  return (HRESULT) 0x8007000EL;
}

HRESULT WINAPI XDinput::GetDeviceStatus(REFGUID rGuidInstance) {
  if(XDINPUT_IS_CONTROLLER_GUID(rGuidInstance)) {
    int player = XDINPUT_GETCONTROLLER(rGuidInstance);
    XINPUT_STATE state;
    printf("[XDinput] GetDeviceStatus\n");

    ZeroMemory(&state, sizeof(XINPUT_STATE));
    return (XInputGetState(player, &state) == ERROR_SUCCESS ? DI_OK : DI_NOTATTACHED);
  }

  if(IsAnsi) {
    if(DinputVersion < 0x0200)
      return ((LPDIRECTINPUTA) diBase)->GetDeviceStatus(rGuidInstance);
    if(DinputVersion < 0x0700)
      return ((LPDIRECTINPUT2A) diBase)->GetDeviceStatus(rGuidInstance);
    if(DinputVersion < 0x0800)
      return ((LPDIRECTINPUT7A) diBase)->GetDeviceStatus(rGuidInstance);
    return ((LPDIRECTINPUT8A) diBase)->GetDeviceStatus(rGuidInstance);
  }

  if(DinputVersion < 0x0200)
    return ((LPDIRECTINPUTW) diBase)->GetDeviceStatus(rGuidInstance);
  if(DinputVersion < 0x0700)
    return ((LPDIRECTINPUT2W) diBase)->GetDeviceStatus(rGuidInstance);
  if(DinputVersion < 0x0800)
    return ((LPDIRECTINPUT7W) diBase)->GetDeviceStatus(rGuidInstance);
  return ((LPDIRECTINPUT8W) diBase)->GetDeviceStatus(rGuidInstance);
}

HRESULT WINAPI XDinput::Initialize(HINSTANCE hInst, DWORD dwVersion) {
  printf("[XDinput] Initialize\n");
  return (HRESULT) 0x8007000EL;
}

HRESULT WINAPI XDinput::RunControlPanel(HWND hwndOwner, DWORD dwFlags) {
  printf("[XDinput] RunControlPanel\n");
  return (HRESULT) 0x8007000EL;
}
