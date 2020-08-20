#include "StdAfx.h"
#include <XInput.h>
#include "JoyConfig.h"

#define CALIB_CENTERVALUE 0
#define CALIB_MINVALUE -1000
#define CALIB_MAXVALUE 1000

JoyConfig::JoyConfig(int index, BOOL isAnsi) {
  isAcquired = 0;
  IsAnsi = isAnsi;
}

JoyConfig::~JoyConfig(void) {
}

///////////////////////////////////////////////////
/// IUnknown implements
ULONG WINAPI JoyConfig::AddRef() {
  return 1;
}

HRESULT WINAPI JoyConfig::QueryInterface(REFIID riid, LPVOID *ppvObject) {
  *ppvObject = NULL;
  return E_NOINTERFACE;
}

ULONG WINAPI JoyConfig::Release() {
  return 0;
}

///////////////////////////////////////////////////
/// IDirectInputJoyConfig8 implements
HRESULT JoyConfig::Acquire(void) {
  isAcquired = 1;
  return DI_OK;
}

HRESULT JoyConfig::Unacquire(void) {
  isAcquired = 0;
  return DI_OK;
}

HRESULT JoyConfig::SetCooperativeLevel(HWND hWnd, DWORD dwFlags) {
  return DI_OK;
}

HRESULT JoyConfig::SendNotify(void) {
  return (isAcquired ? DI_OK : DIERR_NOTACQUIRED);
}

HRESULT JoyConfig::EnumTypes(LPDIJOYTYPECALLBACK callback, LPVOID key) {
  if(callback == NULL)
    return DIERR_INVALIDPARAM;

  callback(L"#XDINPUT", key);
  return DI_OK;
}

HRESULT JoyConfig::GetTypeInfo(LPCWSTR sTypeName, LPDIJOYTYPEINFO joyTypeInfo, DWORD dwFlags) {
  if(sTypeName != L"#XDINPUT")
    return DIERR_NOTFOUND;

  if(dwFlags & DITC_REGHWSETTINGS) {
    joyTypeInfo->hws.dwNumButtons = 12;
    joyTypeInfo->hws.dwFlags = (
      JOY_HWS_ISGAMEPAD | JOY_HWS_HASR |
      JOY_HWS_HASPOV | JOY_HWS_POVISBUTTONCOMBOS | JOY_HWS_POVISPOLL |
      JOY_HWS_XISJ1Y | JOY_HWS_YISJ1X |
      JOY_HWS_RISJ1X | JOY_HWS_RISJ1Y |
      JOY_HWS_HASU | JOY_HWS_HASV
    );
  }

  if(dwFlags & DITC_CLSIDCONFIG)
    joyTypeInfo->clsidConfig = GUID_Joystick;

  if(dwFlags & DITC_DISPLAYNAME)
    swprintf_s(joyTypeInfo->wszDisplayName, MAX_JOYSTRING, L"XDINPUT #%d", thisPlayer);

  if(dwFlags & DITC_CALLOUT)
    swprintf_s(joyTypeInfo->wszCallout, MAX_JOYSTRING, L"XDINPUT%d", thisPlayer);

  return DI_OK;
}

HRESULT JoyConfig::SetTypeInfo(LPCWSTR,LPCDIJOYTYPEINFO,DWORD,LPWSTR) {
  return (isAcquired ? DIERR_READONLY : DIERR_NOTACQUIRED);
}

HRESULT JoyConfig::DeleteType(LPCWSTR) {
  ;return (isAcquired ? DIERR_INVALIDPARAM : DIERR_NOTACQUIRED);
}

HRESULT JoyConfig::GetConfig(UINT iIndex, LPDIJOYCONFIG joyConfig, DWORD dwFlags) {
  if(iIndex >= 4)
    return DIERR_NOMOREITEMS;

  if(dwFlags & DIJC_GUIDINSTANCE) {
    ZeroMemory(&joyConfig->guidInstance, sizeof(GUID));
    XDINPUT_CONTROLLER_GUID(joyConfig->guidInstance, iIndex);
  }

  if(dwFlags & DIJC_REGHWCONFIGTYPE) {
    swprintf_s(joyConfig->wszType, MAX_JOYSTRING, L"XDINPUT #%d", iIndex);
    joyConfig->hwc.hws.dwNumButtons = 12;
    joyConfig->hwc.hws.dwFlags = (
      JOY_HWS_ISGAMEPAD | JOY_HWS_HASR |
      JOY_HWS_HASPOV | JOY_HWS_POVISBUTTONCOMBOS | JOY_HWS_POVISPOLL |
      JOY_HWS_XISJ1Y | JOY_HWS_YISJ1X |
      JOY_HWS_RISJ1X | JOY_HWS_RISJ1Y |
      JOY_HWS_HASU | JOY_HWS_HASV
    );

    joyConfig->hwc.hwv.jrvHardware.jpCenter.dwX = CALIB_CENTERVALUE;
    joyConfig->hwc.hwv.jrvHardware.jpCenter.dwY = CALIB_CENTERVALUE;
    joyConfig->hwc.hwv.jrvHardware.jpCenter.dwZ = CALIB_CENTERVALUE;
    joyConfig->hwc.hwv.jrvHardware.jpCenter.dwR = CALIB_CENTERVALUE;
    joyConfig->hwc.hwv.jrvHardware.jpCenter.dwU = CALIB_CENTERVALUE;
    joyConfig->hwc.hwv.jrvHardware.jpCenter.dwV = CALIB_CENTERVALUE;

    joyConfig->hwc.hwv.jrvHardware.jpMax.dwX = CALIB_MAXVALUE;
    joyConfig->hwc.hwv.jrvHardware.jpMax.dwY = CALIB_MAXVALUE;
    joyConfig->hwc.hwv.jrvHardware.jpMax.dwZ = CALIB_MAXVALUE;
    joyConfig->hwc.hwv.jrvHardware.jpMax.dwR = CALIB_MAXVALUE;
    joyConfig->hwc.hwv.jrvHardware.jpMax.dwU = CALIB_MAXVALUE;
    joyConfig->hwc.hwv.jrvHardware.jpMax.dwV = CALIB_MAXVALUE;

    joyConfig->hwc.hwv.jrvHardware.jpMin.dwX = CALIB_MINVALUE;
    joyConfig->hwc.hwv.jrvHardware.jpMin.dwY = CALIB_MINVALUE;
    joyConfig->hwc.hwv.jrvHardware.jpMin.dwZ = CALIB_MINVALUE;
    joyConfig->hwc.hwv.jrvHardware.jpMin.dwR = CALIB_MINVALUE;
    joyConfig->hwc.hwv.jrvHardware.jpMin.dwU = CALIB_MINVALUE;
    joyConfig->hwc.hwv.jrvHardware.jpMin.dwV = CALIB_MINVALUE;
    
    joyConfig->hwc.hwv.dwPOVValues[JOY_POVVAL_FORWARD] = 0;
    joyConfig->hwc.hwv.dwPOVValues[JOY_POVVAL_RIGHT] = 90;
    joyConfig->hwc.hwv.dwPOVValues[JOY_POVVAL_BACKWARD] = 180;
    joyConfig->hwc.hwv.dwPOVValues[JOY_POVVAL_LEFT] = 270;

    joyConfig->hwc.hwv.dwCalFlags = (
      JOY_ISCAL_XY | JOY_ISCAL_R | JOY_ISCAL_U | JOY_ISCAL_V | JOY_ISCAL_POV
    );

    joyConfig->hwc.dwUsageSettings = (JOY_US_HASRUDDER | JOY_US_PRESENT | JOY_US_ISOEM);
    joyConfig->hwc.dwType = JOY_HW_CUSTOM;
  }

  if(dwFlags & DIJC_GAIN)
    joyConfig->dwGain = 0;

  if(dwFlags & DIJC_CALLOUT)
    swprintf_s(joyConfig->wszCallout, MAX_JOYSTRING, L"XDINPUT%d", iIndex);

  return DI_OK;
}

HRESULT JoyConfig::SetConfig(UINT,LPCDIJOYCONFIG,DWORD) {
  return (isAcquired ? DI_OK : DIERR_NOTACQUIRED);
}

HRESULT JoyConfig::DeleteConfig(UINT) {
  return (isAcquired ? DI_OK : DIERR_NOTACQUIRED);
}

HRESULT JoyConfig::GetUserValues(LPDIJOYUSERVALUES joyUserValues, DWORD dwFlags) {
  if(dwFlags & DIJU_USERVALUES) {
    joyUserValues->ruv.dwTimeOut = 10;

    joyUserValues->ruv.jrvRanges.jpCenter.dwX = CALIB_CENTERVALUE;
    joyUserValues->ruv.jrvRanges.jpCenter.dwY = CALIB_CENTERVALUE;
    joyUserValues->ruv.jrvRanges.jpCenter.dwZ = CALIB_CENTERVALUE;
    joyUserValues->ruv.jrvRanges.jpCenter.dwR = CALIB_CENTERVALUE;
    joyUserValues->ruv.jrvRanges.jpCenter.dwU = CALIB_CENTERVALUE;
    joyUserValues->ruv.jrvRanges.jpCenter.dwV = CALIB_CENTERVALUE;

    joyUserValues->ruv.jrvRanges.jpMax.dwX = CALIB_MAXVALUE;
    joyUserValues->ruv.jrvRanges.jpMax.dwY = CALIB_MAXVALUE;
    joyUserValues->ruv.jrvRanges.jpMax.dwZ = CALIB_MAXVALUE;
    joyUserValues->ruv.jrvRanges.jpMax.dwR = CALIB_MAXVALUE;
    joyUserValues->ruv.jrvRanges.jpMax.dwU = CALIB_MAXVALUE;
    joyUserValues->ruv.jrvRanges.jpMax.dwV = CALIB_MAXVALUE;

    joyUserValues->ruv.jrvRanges.jpMin.dwX = CALIB_MINVALUE;
    joyUserValues->ruv.jrvRanges.jpMin.dwY = CALIB_MINVALUE;
    joyUserValues->ruv.jrvRanges.jpMin.dwZ = CALIB_MINVALUE;
    joyUserValues->ruv.jrvRanges.jpMin.dwR = CALIB_MINVALUE;
    joyUserValues->ruv.jrvRanges.jpMin.dwU = CALIB_MINVALUE;
    joyUserValues->ruv.jrvRanges.jpMin.dwV = CALIB_MINVALUE;

    joyUserValues->ruv.jpDeadZone.dwX = 20;
    joyUserValues->ruv.jpDeadZone.dwY = 20;
    joyUserValues->ruv.jpDeadZone.dwZ = 20;
    joyUserValues->ruv.jpDeadZone.dwR = 20;
    joyUserValues->ruv.jpDeadZone.dwU = 20;
    joyUserValues->ruv.jpDeadZone.dwV = 20;
  }

  if(dwFlags & DIJU_GLOBALDRIVER)
    swprintf_s(joyUserValues->wszGlobalDriver, MAX_JOYSTRING, L"XDINPUT%d", thisPlayer);

  return DI_OK;
}

HRESULT JoyConfig::SetUserValues(LPCDIJOYUSERVALUES,DWORD) {
  return (isAcquired ? DI_OK : DIERR_NOTACQUIRED);
}

HRESULT JoyConfig::AddNewHardware(HWND hWnd, REFGUID rGuid) {
  return DI_OK;
}

HRESULT JoyConfig::OpenTypeKey(LPCWSTR,DWORD,PHKEY) {
  return (isAcquired ? DIERR_INVALIDPARAM : DIERR_NOTACQUIRED);
}

HRESULT JoyConfig::OpenAppStatusKey(PHKEY) {
  return DIERR_NOTFOUND;
}

///////////////////////////////////////////////////
/// JoyConfigLegacy class
JoyConfigLegacy::JoyConfigLegacy(int index, BOOL isAnsi) : JoyConfig(index, isAnsi) {}

JoyConfigLegacy::~JoyConfigLegacy(void) {}

///////////////////////////////////////////////////
/// IUnknown implements
ULONG WINAPI JoyConfigLegacy::AddRef() {
  return JoyConfig::AddRef();
}

HRESULT WINAPI JoyConfigLegacy::QueryInterface(REFIID riid, LPVOID *ppvObject) {
  return JoyConfig::QueryInterface(riid, ppvObject);
}

ULONG WINAPI JoyConfigLegacy::Release() {
  return JoyConfig::Release();
}

///////////////////////////////////////////////////
/// IDirectInputJoyConfig implements
HRESULT WINAPI JoyConfigLegacy::Acquire() {
  return JoyConfig::Acquire();
}

HRESULT WINAPI JoyConfigLegacy::Unacquire() {
  return JoyConfig::Unacquire();
}

HRESULT WINAPI JoyConfigLegacy::SetCooperativeLevel(HWND hWnd, DWORD dwFlags) {
  return JoyConfig::SetCooperativeLevel(hWnd, dwFlags);
}

HRESULT WINAPI JoyConfigLegacy::SendNotify() {
  return JoyConfig::SendNotify();
}

HRESULT WINAPI JoyConfigLegacy::EnumTypes(LPDIJOYTYPECALLBACK lpCallback, LPVOID pvRef) {
  return JoyConfig::EnumTypes(lpCallback, pvRef);
}

HRESULT WINAPI JoyConfigLegacy::GetTypeInfo(LPCWSTR lpszName, LPDIJOYTYPEINFO lpdiJoyTypeInfo, DWORD dwFlags) {
  return JoyConfig::GetTypeInfo(lpszName, lpdiJoyTypeInfo, dwFlags);
}

HRESULT WINAPI JoyConfigLegacy::SetTypeInfo(LPCWSTR lpszName, LPCDIJOYTYPEINFO lpdiJoyTypeInfo, DWORD dwFlags) {
  return JoyConfig::SetTypeInfo(lpszName, lpdiJoyTypeInfo, dwFlags, L"");
}

HRESULT WINAPI JoyConfigLegacy::DeleteType(LPCWSTR lpszName) {
  return JoyConfig::DeleteType(lpszName);
}

HRESULT WINAPI JoyConfigLegacy::GetConfig(UINT iIndex, LPDIJOYCONFIG lpdiJoyConfig, DWORD dwFlags) {
  return JoyConfig::GetConfig(iIndex, lpdiJoyConfig, dwFlags);
}

HRESULT WINAPI JoyConfigLegacy::SetConfig(UINT iIndex, LPCDIJOYCONFIG lpdiJoyConfig, DWORD dwFlags) {
  return JoyConfig::SetConfig(iIndex, lpdiJoyConfig, dwFlags);
}

HRESULT WINAPI JoyConfigLegacy::DeleteConfig(UINT iIndex) {
  return JoyConfig::DeleteConfig(iIndex);
}

HRESULT WINAPI JoyConfigLegacy::GetUserValues(LPDIJOYUSERVALUES lpdiUserValues, DWORD dwFlags) {
  return JoyConfig::GetUserValues(lpdiUserValues, dwFlags);
}

HRESULT WINAPI JoyConfigLegacy::SetUserValues(LPCDIJOYUSERVALUES lpdiUserValues, DWORD dwFlags) {
  return JoyConfig::SetUserValues(lpdiUserValues, dwFlags);
}

HRESULT WINAPI JoyConfigLegacy::AddNewHardware(HWND hWnd, REFGUID rGuid) {
  return JoyConfig::AddNewHardware(hWnd, rGuid);
}

HRESULT WINAPI JoyConfigLegacy::OpenTypeKey(LPCWSTR lpszName, DWORD dwFlags, PHKEY lpHkey) {
  return JoyConfig::OpenTypeKey(lpszName, dwFlags, lpHkey);
}

HRESULT WINAPI JoyConfigLegacy::OpenConfigKey(UINT, DWORD, PHKEY) {
  return DIERR_NOTFOUND;
}