#include "stdafx.h"
#include <dinput.h>
#include <dinputd.h>
#include <xinput.h>

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
    XDinput(int, LPDIRECTINPUT8);
    virtual ~XDinput(void);

  private:
    LPDIRECTINPUT8 diBase;
    int controllerCount;
    BOOL IsAnsi;
};

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
    XDinput7(int, LPDIRECTINPUT7);
    virtual ~XDinput7(void);
};
