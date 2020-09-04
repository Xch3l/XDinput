#pragma once
#include "stdafx.h"
#include <dinput.h>
#include <dinputd.h>

class XDinput : public IDirectInput8 {
  public:
    // IUnknown implements
    HRESULT WINAPI QueryInterface(REFIID, LPVOID*);
    ULONG WINAPI AddRef();
    ULONG WINAPI Release();

    // IDirectInput8 implements
    HRESULT WINAPI CreateDevice(REFGUID, LPDIRECTINPUTDEVICE8*, LPUNKNOWN);
    HRESULT WINAPI EnumDevices(DWORD, LPDIENUMDEVICESCALLBACK, LPVOID, DWORD);
    HRESULT WINAPI GetDeviceStatus(REFGUID);
    HRESULT WINAPI RunControlPanel(HWND, DWORD);
    HRESULT WINAPI Initialize(HINSTANCE, DWORD);
    HRESULT WINAPI FindDevice(REFGUID, LPCTSTR, LPGUID);
    HRESULT WINAPI EnumDevicesBySemantics(LPCWSTR, LPDIACTIONFORMAT, LPDIENUMDEVICESBYSEMANTICSCB, LPVOID, DWORD);
    HRESULT WINAPI ConfigureDevices(LPDICONFIGUREDEVICESCALLBACK, LPDICONFIGUREDEVICESPARAMS, DWORD, LPVOID);

    // this class
    XDinput();
    XDinput(int, LPVOID);
    virtual ~XDinput(void);
    int controllerCount;

  private:
    LPVOID diBase;
    BOOL IsAnsi;
};
