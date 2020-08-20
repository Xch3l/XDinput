#pragma once
#include <dinput.h>
#include <dinputd.h>

class JoyConfig : public IDirectInputJoyConfig8 {
  public:
    // IUnknown implements
    ULONG WINAPI AddRef();
    HRESULT WINAPI QueryInterface(REFIID, LPVOID*);
    ULONG WINAPI Release();

    // IDirectInputJoyConfig8 implements
    HRESULT WINAPI Acquire(void);
    HRESULT WINAPI Unacquire(void);
    HRESULT WINAPI SetCooperativeLevel(HWND, DWORD);
    HRESULT WINAPI SendNotify(void);
    HRESULT WINAPI EnumTypes(LPDIJOYTYPECALLBACK, LPVOID);
    HRESULT WINAPI GetTypeInfo(LPCWSTR, LPDIJOYTYPEINFO, DWORD);
    HRESULT WINAPI SetTypeInfo(LPCWSTR, LPCDIJOYTYPEINFO, DWORD, LPWSTR);
    HRESULT WINAPI DeleteType(LPCWSTR);
    HRESULT WINAPI GetConfig(UINT, LPDIJOYCONFIG, DWORD);
    HRESULT WINAPI SetConfig(UINT, LPCDIJOYCONFIG, DWORD);
    HRESULT WINAPI DeleteConfig(UINT);
    HRESULT WINAPI GetUserValues(LPDIJOYUSERVALUES, DWORD);
    HRESULT WINAPI SetUserValues(LPCDIJOYUSERVALUES, DWORD);
    HRESULT WINAPI AddNewHardware(HWND, REFGUID);
    HRESULT WINAPI OpenTypeKey(LPCWSTR, DWORD, PHKEY);
    HRESULT WINAPI OpenAppStatusKey(PHKEY);

    // This class
    JoyConfig(int, BOOL);
    virtual ~JoyConfig(void);

  private:
    BOOL IsAnsi;
    int isAcquired;
    int thisPlayer;
    XINPUT_STATE controller;
};

class JoyConfigLegacy : public IDirectInputJoyConfig, public JoyConfig {
  public:
    // IUnknown implements
    ULONG WINAPI AddRef();
    HRESULT WINAPI QueryInterface(REFIID, LPVOID*);
    ULONG WINAPI Release();

    // IDirectInputJoyConfig implements
    HRESULT WINAPI Acquire(void);
    HRESULT WINAPI Unacquire(void);
    HRESULT WINAPI SetCooperativeLevel(HWND, DWORD);
    HRESULT WINAPI SendNotify(void);
    HRESULT WINAPI EnumTypes(LPDIJOYTYPECALLBACK, LPVOID);
    HRESULT WINAPI GetTypeInfo(LPCWSTR, LPDIJOYTYPEINFO, DWORD);
    HRESULT WINAPI SetTypeInfo(LPCWSTR, LPCDIJOYTYPEINFO, DWORD);
    HRESULT WINAPI DeleteType(LPCWSTR);
    HRESULT WINAPI GetConfig(UINT, LPDIJOYCONFIG, DWORD);
    HRESULT WINAPI SetConfig(UINT, LPCDIJOYCONFIG, DWORD);
    HRESULT WINAPI DeleteConfig(UINT);
    HRESULT WINAPI GetUserValues(LPDIJOYUSERVALUES, DWORD);
    HRESULT WINAPI SetUserValues(LPCDIJOYUSERVALUES, DWORD);
    HRESULT WINAPI AddNewHardware(HWND, REFGUID);
    HRESULT WINAPI OpenTypeKey(LPCWSTR, DWORD, PHKEY);
    HRESULT WINAPI OpenConfigKey(UINT, DWORD, PHKEY);

    // This class
    JoyConfigLegacy(int, BOOL);
    virtual ~JoyConfigLegacy(void);
};