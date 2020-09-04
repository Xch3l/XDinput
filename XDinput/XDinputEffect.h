#pragma once
#include "dinput.h"

class XDinputEffect : public IDirectInputEffect {
  public:
    // IUnknown implements
    ULONG WINAPI AddRef();
    HRESULT WINAPI QueryInterface(REFIID, LPVOID*);
    ULONG WINAPI Release();

    // IDirectInputEffect implements
    virtual HRESULT WINAPI Initialize(HINSTANCE, DWORD, REFGUID);
    virtual HRESULT WINAPI GetEffectGuid(LPGUID);
    virtual HRESULT WINAPI GetParameters(LPDIEFFECT,DWORD);
    virtual HRESULT WINAPI SetParameters(LPCDIEFFECT,DWORD);
    virtual HRESULT WINAPI Start(DWORD,DWORD);
    virtual HRESULT WINAPI Stop();
    virtual HRESULT WINAPI GetEffectStatus(LPDWORD);
    virtual HRESULT WINAPI Download();
    virtual HRESULT WINAPI Unload();
    virtual HRESULT WINAPI Escape(LPDIEFFESCAPE);

    // This class
    XDinputEffect(REFGUID, const CHAR*, const WCHAR*);
    virtual ~XDinputEffect(void);
    DIEFFECT EffectParameters;
    BOOL Playing;
    BOOL Enabled;

    static const int Version = 0x20200825;

  private:
    GUID  Guid;
    CHAR  AnsiName[MAX_PATH];
    WCHAR WideName[MAX_PATH];
};

