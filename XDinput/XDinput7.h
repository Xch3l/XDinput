#pragma once
#include "stdafx.h"
#include "XDinput.h"

class XDinput7 : public IDirectInput7, public XDinput {
  public:
    // IUnknown implements
    HRESULT WINAPI QueryInterface(REFIID, LPVOID*);
    ULONG WINAPI AddRef();
    ULONG WINAPI Release();

    // IDirectInput7 implements
    HRESULT WINAPI CreateDevice(REFGUID, LPDIRECTINPUTDEVICE*, LPUNKNOWN);
    HRESULT WINAPI EnumDevices(DWORD, LPDIENUMDEVICESCALLBACK, LPVOID, DWORD);
    HRESULT WINAPI GetDeviceStatus(REFGUID);
    HRESULT WINAPI RunControlPanel(HWND, DWORD);
    HRESULT WINAPI Initialize(HINSTANCE, DWORD);
    HRESULT WINAPI FindDevice(REFGUID, LPCTSTR, LPGUID);
    HRESULT WINAPI CreateDeviceEx(REFGUID, REFIID, LPVOID*, LPUNKNOWN);

    // This class
    XDinput7(int, LPVOID);
    virtual ~XDinput7(void);
};