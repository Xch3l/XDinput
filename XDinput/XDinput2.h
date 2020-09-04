#pragma once
#include "stdafx.h"
#include "XDinput.h"

class XDinput2 : public IDirectInput2 {
  public:
    // IUnknown implements
    HRESULT WINAPI QueryInterface(REFIID, LPVOID*);
    ULONG WINAPI AddRef();
    ULONG WINAPI Release();

    // IDirectInput2 implements
    HRESULT WINAPI CreateDevice(REFGUID, LPDIRECTINPUTDEVICE*, LPUNKNOWN);
    HRESULT WINAPI EnumDevices(DWORD, LPDIENUMDEVICESCALLBACK, LPVOID, DWORD);
    HRESULT WINAPI GetDeviceStatus(REFGUID);
    HRESULT WINAPI RunControlPanel(HWND, DWORD);
    HRESULT WINAPI Initialize(HINSTANCE, DWORD);
    HRESULT WINAPI FindDevice(REFGUID, LPCTSTR, LPGUID);

    // This class
    XDinput2(int, LPVOID);
    virtual ~XDinput2(void);
    XDinput* xdiBase;
};