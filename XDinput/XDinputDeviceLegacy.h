#pragma once
#include "stdafx.h"
#include "XDInputDevice.h"

class XDinputDeviceLegacy : public IDirectInputDevice {
  public:
    // IUnknown implements
    ULONG WINAPI AddRef();
    HRESULT WINAPI QueryInterface(REFIID, LPVOID*);
    ULONG WINAPI Release();

    // IDirectInputDevice implements
    HRESULT WINAPI GetCapabilities(LPDIDEVCAPS);
    HRESULT WINAPI EnumObjects(LPDIENUMDEVICEOBJECTSCALLBACKW, LPVOID, DWORD);
    HRESULT WINAPI GetProperty(REFGUID, LPDIPROPHEADER);
    HRESULT WINAPI SetProperty(REFGUID, LPCDIPROPHEADER);
    HRESULT WINAPI Acquire(void);
    HRESULT WINAPI Unacquire(void);
    HRESULT WINAPI GetDeviceState(DWORD, LPVOID);
    HRESULT WINAPI GetDeviceData(DWORD, LPDIDEVICEOBJECTDATA, LPDWORD, DWORD);
    HRESULT WINAPI SetDataFormat(LPCDIDATAFORMAT);
    HRESULT WINAPI SetEventNotification(HANDLE);
    HRESULT WINAPI SetCooperativeLevel(HWND, DWORD);
    HRESULT WINAPI GetObjectInfo(LPDIDEVICEOBJECTINSTANCEW, DWORD, DWORD);
    HRESULT WINAPI GetDeviceInfo(LPDIDEVICEINSTANCEW);
    HRESULT WINAPI RunControlPanel(HWND, DWORD);
    HRESULT WINAPI Initialize(HINSTANCE, DWORD, REFGUID);

    // This class
    XDinputDeviceLegacy(int, BOOL);
    virtual ~XDinputDeviceLegacy(void);

  private:
    XDinputDevice* xdiBase;
};