#pragma once
#include <XInput.h>
#include "XDinputEffect.h"
#include "dinput.h"

class XDinputConstantForceEffect : public XDinputEffect {
  public:
    // XDinputEffect overrides
    HRESULT WINAPI SetParameters(LPCDIEFFECT,DWORD) override;
    HRESULT WINAPI Start(DWORD,DWORD) override;
    HRESULT WINAPI Stop() override;

    // This class
    XDinputConstantForceEffect(int);
    virtual ~XDinputConstantForceEffect();

  private:
    XINPUT_VIBRATION vibration;
    int thisIndex;
};

