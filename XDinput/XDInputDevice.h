#pragma once
#include "stdafx.h"
#include <dinput.h>
#include "XDinputEffect.h"

#define MAX_EFFECTS 32

class XDinputDevice : public IDirectInputDevice8 {
  public:
    // IUnknown implements
    ULONG WINAPI AddRef();
    HRESULT WINAPI QueryInterface(REFIID, LPVOID*);
    ULONG WINAPI Release();

    // IDirectInputDevice8 implements
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
    HRESULT WINAPI CreateEffect(REFGUID, LPCDIEFFECT, LPDIRECTINPUTEFFECT *, LPUNKNOWN);
    HRESULT WINAPI EnumEffects(LPDIENUMEFFECTSCALLBACKW, LPVOID, DWORD);
    HRESULT WINAPI GetEffectInfo(LPDIEFFECTINFOW, REFGUID);
    HRESULT WINAPI GetForceFeedbackState(LPDWORD);
    HRESULT WINAPI SendForceFeedbackCommand(DWORD);
    HRESULT WINAPI EnumCreatedEffectObjects(LPDIENUMCREATEDEFFECTOBJECTSCALLBACK, LPVOID, DWORD);
    HRESULT WINAPI Escape(LPDIEFFESCAPE);
    HRESULT WINAPI Poll(void);
    HRESULT WINAPI SendDeviceData(DWORD, LPCDIDEVICEOBJECTDATA, LPDWORD, DWORD);
    HRESULT WINAPI EnumEffectsInFile(LPCWSTR, LPDIENUMEFFECTSINFILECALLBACK, LPVOID, DWORD);
    HRESULT WINAPI WriteEffectToFile(LPCWSTR, DWORD, LPDIFILEEFFECT, DWORD);
    HRESULT WINAPI BuildActionMap(LPDIACTIONFORMATW, LPCWSTR, DWORD);
    HRESULT WINAPI SetActionMap(LPDIACTIONFORMATW, LPCWSTR, DWORD);
    HRESULT WINAPI GetImageInfo(LPDIDEVICEIMAGEINFOHEADERW);

    // this class
    XDinputDevice(int, BOOL);
    virtual ~XDinputDevice(void);
    void CenterAxes(void);

  private:
    int referenceCount, effectsUsed;
    XDinputEffect* thisEffect[MAX_EFFECTS];
    DIOBJECTDATAFORMAT* dataFormatValues;
    DWORD dataFormat;

    XINPUT_STATE state;
    int thisIndex;
    BOOL IsAcquired;
    BOOL IsAnsi;
    BOOL IsPolled;

    BOOL absoluteMode;
    int AxisRangeMin, AxisRangeMax;
    float restingLX, restingLY;
    float restingRX, restingRY;
    float restingLT, restingRT;
};
