#pragma once
#include <XInput.h>
#include "dinput.h"

class XDinputEffect : public IDirectInputEffect {
  public:
    // IUnknown implements
    ULONG WINAPI AddRef();
    HRESULT WINAPI QueryInterface(REFIID, LPVOID*);
    ULONG WINAPI Release();

    // IDirectInputEffect implements
    HRESULT WINAPI Initialize(HINSTANCE, DWORD, REFGUID);
    HRESULT WINAPI GetEffectGuid(LPGUID);
    HRESULT WINAPI GetParameters(LPDIEFFECT,DWORD);
    HRESULT WINAPI SetParameters(LPCDIEFFECT,DWORD);
    HRESULT WINAPI Start(DWORD,DWORD);
    HRESULT WINAPI Stop();
    HRESULT WINAPI GetEffectStatus(LPDWORD);
    HRESULT WINAPI Download();
    HRESULT WINAPI Unload();
    HRESULT WINAPI Escape(LPDIEFFESCAPE);

    // This class
    XDinputEffect(int);
    virtual ~XDinputEffect();

  private:
    XINPUT_VIBRATION vibration;
    int thisIndex;
};

