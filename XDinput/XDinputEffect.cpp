#include "StdAfx.h"
#include "XDinputEffect.h"
#include <XInput.h>
#include <math.h>

XDinputEffect::XDinputEffect(int i) {
  printf("Created FFB Const Effect for player %d\n", i);
  ZeroMemory(&vibration, sizeof(XINPUT_VIBRATION));
  thisIndex = i;
  leftMotor[i] = rightMotor[i] = 0;
}

XDinputEffect::~XDinputEffect() {
  printf("Destroyed FFB Const Effect for player %d\n", thisIndex);
}

///////////////////////////////////////////////////
/// IUnknown implements
ULONG XDinputEffect::AddRef() {
  return 1;
}

HRESULT XDinputEffect::QueryInterface(REFIID riid, LPVOID *ppvObject) {
  *ppvObject = NULL;
  return E_NOINTERFACE;
}

ULONG XDinputEffect::Release() {
  return 0;
}

///////////////////////////////////////////////////
// IDirectInputEffect implements
HRESULT XDinputEffect::Initialize(HINSTANCE hInst, DWORD dwFlags, REFGUID rGuid) {
  printf("Initializing FFB Effect\n");
  printf("  hInst: %08X\n", hInst);
  printf("  dwFlags: %08X\n", dwFlags);
  printf("  rGuid: {%08X-04X-%04X", rGuid.Data1, rGuid.Data2, rGuid.Data3);
  for(int i = 0; i < 8; i++) {
    printf("%02X", rGuid.Data4[i]);
    if(i == 1) printf("-");
  }
  puts("}\n");

  return DI_OK;
}

HRESULT XDinputEffect::GetEffectGuid(LPGUID lpGuid) {
  *lpGuid = GUID_ConstantForce;
  return DI_OK;
}

HRESULT XDinputEffect::GetParameters(LPDIEFFECT lpdiEffect, DWORD dwFlags) {
  return E_FAIL;
}

HRESULT XDinputEffect::SetParameters(LPCDIEFFECT lpdiEffect, DWORD dwFlags) {
  ZeroMemory(&vibration, sizeof(XINPUT_VIBRATION));

  if(!(dwFlags & DIEP_NORESTART))
    XInputSetState(thisIndex, &vibration);

  /*if(dwFlags & DIEP_SAMPLEPERIOD)
    ; //*/

  if(dwFlags & DIEP_TYPESPECIFICPARAMS) {
    LONG* dirXY = (LONG*) lpdiEffect->rglDirection;
    DICONSTANTFORCE f = *((LPDICONSTANTFORCE) lpdiEffect->lpvTypeSpecificParams);
    FLOAT magnitude = (FLOAT) f.lMagnitude;
    WORD forceLo, forceHi;

    if(magnitude < -DI_FFNOMINALMAX) magnitude = -DI_FFNOMINALMAX; else
    if(magnitude > +DI_FFNOMINALMAX) magnitude = +DI_FFNOMINALMAX;
    magnitude = magnitude / (FLOAT) DI_FFNOMINALMAX;

    if(lpdiEffect->cAxes == 1) {
      forceLo = (WORD) abs(lerp(0, 65535, magnitude));
      forceHi = forceLo;
    } else
    if(lpdiEffect->cAxes >= 2) {
      forceLo = (WORD) abs(lerp(0, 65535, (dirXY[0] / (FLOAT) DI_FFNOMINALMAX) * magnitude));
      forceHi = (WORD) abs(lerp(0, 65535, (dirXY[1] / (FLOAT) DI_FFNOMINALMAX) * magnitude));
    }

    printf("VIB % 6d, % 6d; MAG %2.4f      \r", forceLo, forceHi, magnitude);
    leftMotor[thisIndex] = vibration.wLeftMotorSpeed = forceLo;
    rightMotor[thisIndex] = vibration.wRightMotorSpeed = forceHi;
  }

  if(dwFlags & DIEP_START)
    return (XInputSetState(thisIndex, &vibration) == ERROR_DEVICE_NOT_CONNECTED ? DIERR_INPUTLOST : DI_OK);

  return DI_OK;
}

HRESULT XDinputEffect::Start(DWORD dwIterations, DWORD dwFlags) {
  return (XInputSetState(thisIndex, &vibration) == ERROR_DEVICE_NOT_CONNECTED ? DIERR_INPUTLOST : DI_OK);
}

HRESULT XDinputEffect::Stop() {
  ZeroMemory(&vibration, sizeof(XINPUT_VIBRATION));
  return (XInputSetState(thisIndex, &vibration) == ERROR_DEVICE_NOT_CONNECTED ? DIERR_INPUTLOST : DI_OK);
}

HRESULT XDinputEffect::GetEffectStatus(LPDWORD) {
  return E_FAIL;
}

HRESULT XDinputEffect::Download() {
  return E_FAIL;
}

HRESULT XDinputEffect::Unload() {
  return E_FAIL;
}

HRESULT XDinputEffect::Escape(LPDIEFFESCAPE) {
  return E_FAIL;
}
