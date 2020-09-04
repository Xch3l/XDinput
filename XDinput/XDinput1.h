#pragma once
#include "stdafx.h"
#include "XDinput.h"

class XDinput1 : public IDirectInput {
  public:
    // IUnknown implements
    HRESULT WINAPI QueryInterface(REFIID, LPVOID*);
    ULONG WINAPI AddRef();
    ULONG WINAPI Release();

    // IDirectInput implements
    HRESULT WINAPI CreateDevice(REFGUID, LPDIRECTINPUTDEVICE*, LPUNKNOWN);
    HRESULT WINAPI EnumDevices(DWORD, LPDIENUMDEVICESCALLBACK, LPVOID, DWORD);
    HRESULT WINAPI GetDeviceStatus(REFGUID);
    HRESULT WINAPI RunControlPanel(HWND, DWORD);
    HRESULT WINAPI Initialize(HINSTANCE, DWORD);

    // This class
    XDinput1(int, LPVOID);
    virtual ~XDinput1(void);
    XDinput* xdiBase;
};
