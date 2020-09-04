#include "StdAfx.h"
#include <XInput.h>
#include <math.h>
#include "XDinputConstantForceEffect.h"

XDinputConstantForceEffect::XDinputConstantForceEffect(int i) : XDinputEffect(GUID_ConstantForce, XDINPUT_EFFECTNAMEA, XDINPUT_EFFECTNAMEW) {
  ZeroMemory(&vibration, sizeof(XINPUT_VIBRATION));
  thisIndex = i;
  leftMotor[i] = rightMotor[i] = 0;
  Enabled = TRUE;

  DWORD dwAxes[2] = {0};
  LONG lDirections[2] = {0, 1};

  // Fill a DIEFFECT struct to have it ready
  ZeroMemory(&EffectParameters, sizeof(DIEFFECT));
  EffectParameters.dwSize = sizeof(DIEFFECT);
  EffectParameters.dwFlags = DIEFF_CARTESIAN | DIEFF_OBJECTIDS;
  EffectParameters.dwDuration = 0;
  EffectParameters.dwSamplePeriod = 0;
  EffectParameters.dwGain = 0;
  EffectParameters.dwTriggerButton = DIEB_NOTRIGGER;
  EffectParameters.dwTriggerRepeatInterval = 0;
  EffectParameters.cAxes = 2;
  EffectParameters.rgdwAxes = dwAxes;
  EffectParameters.rglDirection = lDirections;
  EffectParameters.lpEnvelope = NULL;
  EffectParameters.cbTypeSpecificParams;
  EffectParameters.lpvTypeSpecificParams;
  EffectParameters.dwStartDelay;
}

XDinputConstantForceEffect::~XDinputConstantForceEffect() {}

///////////////////////////////////////////////////
/// XDinputEffect overrides
HRESULT XDinputConstantForceEffect::SetParameters(LPCDIEFFECT lpdiEffect, DWORD dwFlags) {
  ZeroMemory(&vibration, sizeof(XINPUT_VIBRATION));

  if(!(dwFlags & DIEP_NORESTART))
    XInputSetState(thisIndex, &vibration);

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

#ifdef _DEBUG
    printf("VIB % 6d, % 6d; MAG %2.4f      \r", forceLo, forceHi, magnitude);
#endif

    leftMotor[thisIndex] = vibration.wLeftMotorSpeed = forceLo;
    rightMotor[thisIndex] = vibration.wRightMotorSpeed = forceHi;
  }

  if(dwFlags & DIEP_START)
    return Start(0, 0);

  return DI_OK;
}

HRESULT XDinputConstantForceEffect::Start(DWORD dwIterations, DWORD dwFlags) {
  XDinputEffect::Start(dwIterations, dwFlags);
  return (XInputSetState(thisIndex, &vibration) == ERROR_DEVICE_NOT_CONNECTED ? DIERR_INPUTLOST : DI_OK);
}

HRESULT XDinputConstantForceEffect::Stop() {
  XDinputEffect::Stop();
  ZeroMemory(&vibration, sizeof(XINPUT_VIBRATION));
  return (XInputSetState(thisIndex, &vibration) == ERROR_DEVICE_NOT_CONNECTED ? DIERR_INPUTLOST : DI_OK);
}
