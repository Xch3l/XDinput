#include "StdAfx.h"
#include "XDinputEffect.h"

XDinputEffect::XDinputEffect(REFGUID rGuid, const CHAR* szAnsiName, const WCHAR* szWideName) {
  Guid = rGuid;
  strncpy_s(AnsiName, MAX_PATH, szAnsiName, _TRUNCATE);
  wcsncpy_s(WideName, MAX_PATH, szWideName, _TRUNCATE);

  Playing = FALSE;
  Enabled = TRUE;

  ZeroMemory(&EffectParameters, sizeof(DIEFFECT));
}

XDinputEffect::~XDinputEffect(void) {}

///////////////////////////////////////////////////
/// IUnknown implements
ULONG XDinputEffect::AddRef() {
  return 0;
}

HRESULT XDinputEffect::QueryInterface(REFIID riid, LPVOID *ppvObject) {
  return DIERR_NOINTERFACE;
}

ULONG XDinputEffect::Release() {
  return 0;
}

///////////////////////////////////////////////////
// IDirectInputEffect implements
HRESULT XDinputEffect::Initialize(HINSTANCE hInst, DWORD dwFlags, REFGUID rGuid) {
  return DI_OK;
}

HRESULT XDinputEffect::GetEffectGuid(LPGUID lpGuid) {
  memcpy(lpGuid, &Guid, sizeof(GUID));
  return DI_OK;
}

HRESULT XDinputEffect::GetParameters(LPDIEFFECT lpdiEffect, DWORD dwFlags) {
  if(lpdiEffect->dwSize != sizeof(DIEFFECT) || lpdiEffect->dwSize != sizeof(DIEFFECT_DX6))
    return DIERR_NOTINITIALIZED;

  if(dwFlags & DIEP_AXES) {
    if(lpdiEffect->cAxes < EffectParameters.cAxes) {
      lpdiEffect->cAxes = EffectParameters.cAxes;
      return DIERR_MOREDATA;
    }

    memcpy(lpdiEffect->rgdwAxes, EffectParameters.rgdwAxes, sizeof(DWORD) * EffectParameters.cAxes);
    lpdiEffect->cAxes = EffectParameters.cAxes;
  }

  if(dwFlags & DIEP_DIRECTION) {
    if(lpdiEffect->cAxes < EffectParameters.cAxes) {
      lpdiEffect->cAxes = EffectParameters.cAxes;
      return DIERR_MOREDATA;
    }

    memcpy(lpdiEffect->rglDirection, EffectParameters.rglDirection, sizeof(DWORD) * EffectParameters.cAxes);
    lpdiEffect->dwFlags = (lpdiEffect->dwFlags & 0xFFFFFF8F) | DIEFF_CARTESIAN;
    lpdiEffect->cAxes = EffectParameters.cAxes;
  }

  if(dwFlags & DIEP_DURATION)
    lpdiEffect->dwDuration = EffectParameters.dwDuration;

  if(dwFlags & DIEP_ENVELOPE)
    lpdiEffect->lpEnvelope = EffectParameters.lpEnvelope;

  if(dwFlags & DIEP_GAIN)
    lpdiEffect->dwGain = EffectParameters.dwGain;

  if(dwFlags & DIEP_SAMPLEPERIOD)
    lpdiEffect->dwSamplePeriod = EffectParameters.dwSamplePeriod;

  if(dwFlags & DIEP_STARTDELAY)
    lpdiEffect->dwStartDelay = EffectParameters.dwStartDelay;

  if(dwFlags & DIEP_TRIGGERBUTTON)
    lpdiEffect->dwTriggerButton = EffectParameters.dwTriggerButton;

  if(dwFlags & DIEP_TRIGGERREPEATINTERVAL)
    lpdiEffect->dwTriggerRepeatInterval = EffectParameters.dwTriggerRepeatInterval;

  if(dwFlags & DIEP_TYPESPECIFICPARAMS) {
    if(lpdiEffect->cbTypeSpecificParams < EffectParameters.cbTypeSpecificParams) {
      lpdiEffect->cbTypeSpecificParams = EffectParameters.cbTypeSpecificParams;
      return DIERR_MOREDATA;
    }

    memcpy(lpdiEffect->lpvTypeSpecificParams, EffectParameters.lpvTypeSpecificParams, EffectParameters.cbTypeSpecificParams);
  }

  return DI_OK;
}

HRESULT XDinputEffect::SetParameters(LPCDIEFFECT lpdiEffect, DWORD dwFlags) {
  return DI_OK;
}

HRESULT XDinputEffect::Start(DWORD dwIterations, DWORD dwFlags) {
  Enabled = TRUE;
  Playing = TRUE;
  return DI_OK;
}

HRESULT XDinputEffect::Stop() {
  Playing = FALSE;
  return DI_OK;
}

HRESULT XDinputEffect::GetEffectStatus(LPDWORD pdwFlags) {
  *pdwFlags = DIEGES_PLAYING;
  if(Playing) *pdwFlags |= DIEGES_EMULATED;
  return DI_OK;
}

HRESULT XDinputEffect::Download() {
  return DI_OK;
}

HRESULT XDinputEffect::Unload() {
  return DI_OK;
}

HRESULT XDinputEffect::Escape(LPDIEFFESCAPE) {
  return DI_OK;
}
