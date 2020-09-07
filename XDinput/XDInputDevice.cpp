#include "StdAfx.h"
#include "XDInputDevice.h"
#include "XDinputEffect.h"
#include "XDinputDeviceLegacy.h"
#include "XDinputConstantForceEffect.h"

XDinputDevice::XDinputDevice(int iIndex, BOOL isAnsi) {
  printf("[%d] new XDinputDevice%c\n", iIndex, isAnsi ? 'A' : 'W');

  ZeroMemory(&state, sizeof(XINPUT_STATE));
  XInputGetState(iIndex, &state);

  thisIndex = iIndex;
  IsAcquired = FALSE;
  IsAnsi = isAnsi;
  IsPolled = FALSE;
  absoluteMode = TRUE;

  for(int i = 0; i < MAX_EFFECTS; i++)
    thisEffect[i] = NULL;

  effectsUsed = 0;
  referenceCount = 0;
  dataFormatValues = NULL;
  dataFormat = 0;

  AxisRangeMin = -32768;
  AxisRangeMax = +32767;

  // initialize all 'resting' offsets
  restingLX = restingLY = 0; // left stick
  restingRX = restingRY = 0; // right stick
  restingLT = restingRT = 0; // triggers

  AddRef();
  CenterAxes();
}

XDinputDevice::~XDinputDevice(void) {
  printf("[%d] delete XDinputDevice\n", thisIndex);
}

void XDinputDevice::CenterAxes() {
  XINPUT_STATE s;

  ZeroMemory(&s, sizeof(XINPUT_STATE));
  XInputGetState(thisIndex, &s);

  restingLX = (float) (s.Gamepad.sThumbLX / 32768.0f);
  restingLY = (float) (s.Gamepad.sThumbLY / 32768.0f);
  restingRX = (float) (s.Gamepad.sThumbRX / 32768.0f);
  restingRY = (float) (s.Gamepad.sThumbRY / 32768.0f);
  restingLT = (float) (s.Gamepad.bLeftTrigger / 255.0f);
  restingRT = (float) (s.Gamepad.bRightTrigger / 255.0f);
}

///////////////////////////////////////////////////

extern DWORD DinputVersion;

const int objCount = 21;
const CHAR* objNameA[] = { // tszName (ANSI)
  XDOA_LX, XDOA_LY, XDOA_RX, XDOA_RY, XDOA_LT, XDOA_RT,
  XDOA_AB, XDOA_BB, XDOA_XB, XDOA_YB, XDOA_ST, XDOA_BA,
  XDOA_LB, XDOA_RB, XDOA_LS, XDOA_RS, XDOA_L2, XDOA_R2,
  XDOA_DP, XDOA_LM, XDOA_RM
};
const WCHAR* objNameW[] = { // tszName (WIDE)
  XDOW_LX, XDOW_LY, XDOW_RX, XDOW_RY, XDOW_LT, XDOW_RT,
  XDOW_AB, XDOW_BB, XDOW_XB, XDOW_YB, XDOW_ST, XDOW_BA,
  XDOW_LB, XDOW_RB, XDOW_LS, XDOW_RS, XDOW_L2, XDOW_R2,
  XDOW_DP, XDOW_LM, XDOW_RM
};
const GUID objGUID[] = { // guidType
  GUID_XAxis,  GUID_YAxis,  GUID_RxAxis, GUID_RyAxis, GUID_Slider, GUID_Slider,
  GUID_Button, GUID_Button, GUID_Button, GUID_Button, GUID_Button, GUID_Button,
  GUID_Button, GUID_Button, GUID_Button, GUID_Button, GUID_Button, GUID_Button,
  GUID_POV, XDINPUT_VIBRATOR_GUID, XDINPUT_VIBRATOR_GUID
};
const DWORD objType[] = { // dwType
  DIDFT_ABSAXIS,    DIDFT_ABSAXIS,    DIDFT_ABSAXIS,    DIDFT_ABSAXIS,    DIDFT_ABSAXIS,    DIDFT_ABSAXIS,
  DIDFT_PSHBUTTON,  DIDFT_PSHBUTTON,  DIDFT_PSHBUTTON,  DIDFT_PSHBUTTON,  DIDFT_PSHBUTTON,  DIDFT_PSHBUTTON,
  DIDFT_PSHBUTTON,  DIDFT_PSHBUTTON,  DIDFT_PSHBUTTON,  DIDFT_PSHBUTTON,  DIDFT_PSHBUTTON,  DIDFT_PSHBUTTON,
  DIDFT_POV,        DIDFT_FFACTUATOR, DIDFT_FFACTUATOR
};
const DWORD objFlags[] = { // dwFlags
  DIDOI_ASPECTPOSITION, DIDOI_ASPECTPOSITION, DIDOI_ASPECTPOSITION, DIDOI_ASPECTPOSITION, DIDOI_ASPECTPOSITION, DIDOI_ASPECTPOSITION,
  0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0,
  0, DIDOI_FFACTUATOR, DIDOI_FFACTUATOR
};
const DWORD objOffs[] = { // dwOfs
   0,  4, 12, 16, 24, 28,
  48, 49, 50, 51, 52, 53,
  54, 55, 56, 57, 58, 59,
  32, 36, 40
};
const DWORD objFFMaxForce[] = { // dwFFMaxForce
  0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0,
  0, 65535, 65535
};
const DWORD objFFResolution[] = { // dwFFForceResolution
  0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0,
  0, 1, 1
};
const DWORD objPageAndUsage[] = { // dwUsagePage (HI) & dwUsage (LO)
  0x00010030, 0x00010031, 0x00010033, 0x00010034, 0x00010036, 0x00010036,
  0x00090001, 0x00090002, 0x00090003, 0x00090004, 0x00090005, 0x00090006,
  0x00090007, 0x00090008, 0x00090009, 0x0009000A, 0x0009000B, 0x0009000C,
  0x00010039, 0x000E0001, 0x000E0001
};

// objPageAndUsage values referenced from
//    HID Usages and Definitions Table v1.2
//    https://www.usb.org/sites/default/files/hut1_2.pdf

///////////////////////////////////////////////////
/// IUnknown implements
ULONG WINAPI XDinputDevice::AddRef() {
  printf("[%d] XDinputDevice::AddRef\n", thisIndex);
  return ++referenceCount;
}

HRESULT WINAPI XDinputDevice::QueryInterface(REFIID riid, LPVOID *ppvObject) {
  printf("[%d] XDinputDevice::QueryInterface\n", thisIndex);
  *ppvObject = NULL;

  if(riid == IID_IDirectInputDeviceA) {
    puts("  IID_IDirectInputDeviceA");
    *ppvObject = (LPDIRECTINPUTDEVICEA) new XDinputDeviceLegacy(thisIndex, TRUE);
  } else
  if(riid == IID_IDirectInputDeviceW) {
    puts("  IID_IDirectInputDeviceW");
    *ppvObject = (LPDIRECTINPUTDEVICEW) new XDinputDeviceLegacy(thisIndex, FALSE);
  } else
  if(riid == IID_IDirectInputDevice2A) {
    puts("  IID_IDirectInputDevice2A");
    *ppvObject = (LPDIRECTINPUTDEVICE2A) new XDinputDevice(thisIndex, TRUE);
  } else
  if(riid == IID_IDirectInputDevice2W) {
    puts("  IID_IDirectInputDevice2W");
    *ppvObject = (LPDIRECTINPUTDEVICEW*) new XDinputDevice(thisIndex, FALSE);
  } else
  if(riid == IID_IDirectInputDevice7A) {
    puts("  IID_IDirectInputDevice7A");
    *ppvObject = (LPDIRECTINPUTDEVICE7A) new XDinputDevice(thisIndex, TRUE);
  } else
  if(riid == IID_IDirectInputDevice7W) {
    puts("  IID_IDirectInputDevice7W");
    *ppvObject = (LPDIRECTINPUTDEVICE7W) new XDinputDevice(thisIndex, FALSE);
  } else
  if(riid == IID_IDirectInputDevice8A) {
    puts("  IID_IDirectInputDevice8A");
    *ppvObject = (LPDIRECTINPUTDEVICE8A) new XDinputDevice(thisIndex, TRUE);
  } else
  if(riid == IID_IDirectInputDevice8W) {
    puts("  IID_IDirectInputDevice8W");
    *ppvObject = (LPDIRECTINPUTDEVICE8W) new XDinputDevice(thisIndex, FALSE);
  } else
  if(riid == IID_IDirectInputEffect) {
    puts("  IID_IDirectInputEffect");
    *ppvObject = (LPVOID) CreateEffect(GUID_ConstantForce, NULL, (LPDIRECTINPUTEFFECT*) ppvObject, NULL);
  }

  if(*ppvObject == NULL)
    return E_NOINTERFACE;

  AddRef();
  return S_OK;
}

ULONG WINAPI XDinputDevice::Release() {
  printf("[%d] XDinputDevice::Release\n", thisIndex);
  int n = --referenceCount;

  if(n <= 0)
    delete this;

  return n;
}

///////////////////////////////////////////////////
/// IDirectInputDevice8 implements
HRESULT XDinputDevice::GetCapabilities(LPDIDEVCAPS lpdiDevCaps) {
  printf("[%d] GetCapabilities\n", thisIndex);

  if(lpdiDevCaps->dwSize != sizeof(DIDEVCAPS) && lpdiDevCaps->dwSize != sizeof(DIDEVCAPS_DX3))
    return DIERR_NOTINITIALIZED;

  XINPUT_CAPABILITIES caps;

  ZeroMemory(&caps, sizeof(XINPUT_CAPABILITIES));

  lpdiDevCaps->dwFlags = DIDC_EMULATED;

  if(XInputGetCapabilities(thisIndex, XINPUT_FLAG_GAMEPAD, &caps) == ERROR_SUCCESS)
    lpdiDevCaps->dwFlags |= DIDC_ATTACHED;

  lpdiDevCaps->dwDevType = (DinputVersion <= 0x0700 ? DI8DEVCLASS_GAMECTRL : DI8DEVTYPE_GAMEPAD);
  lpdiDevCaps->dwAxes = 4;
  lpdiDevCaps->dwButtons = 12;
  lpdiDevCaps->dwPOVs = 1;

  if(DinputVersion >= 0x0500) {
    lpdiDevCaps->dwFlags |= DIDC_FORCEFEEDBACK;
    lpdiDevCaps->dwFFSamplePeriod = 1000;
    lpdiDevCaps->dwFFMinTimeResolution = 1000;
    lpdiDevCaps->dwFirmwareRevision = 0x20200825;
    lpdiDevCaps->dwHardwareRevision = 0xB000B1E5;
    lpdiDevCaps->dwFFDriverVersion = XDinputEffect::Version;
  }

  return DI_OK;
}

HRESULT XDinputDevice::EnumObjects(LPDIENUMDEVICEOBJECTSCALLBACKW lpCallback, LPVOID pvRef, DWORD dwFlags) {
  printf("[%d] EnumObjects\n", thisIndex);

  for(int i = 0; i < objCount; i++) {
    if(IsAnsi) {
      DIDEVICEOBJECTINSTANCEA doi;

      if(DinputVersion >= 0x0500)
        doi.dwSize = sizeof(DIDEVICEOBJECTINSTANCEA);
      else
        doi.dwSize = sizeof(DIDEVICEOBJECTINSTANCE_DX3A);

      doi.guidType = objGUID[i];
      doi.dwOfs = objOffs[i];
      doi.dwType = (thisIndex << 8) | objType[i];
      doi.dwFlags = objFlags[i];
      sprintf_s(doi.tszName, MAX_PATH, objNameA[i]);

      if(DinputVersion >= 0x0500) {
        doi.dwFFMaxForce = objFFMaxForce[i];
        doi.dwFFForceResolution = objFFResolution[i];
        doi.wCollectionNumber = thisIndex;
        doi.wDesignatorIndex = 0;
        doi.wUsagePage = (WORD) (objPageAndUsage[i] >> 16) & 0xFFFF;
        doi.wUsage = (WORD) (objPageAndUsage[i] & 0xFFFF);
        doi.dwDimension = 0;
        doi.wExponent = 0;
        doi.wReportId = 0;
      }

      if(((LPDIENUMDEVICEOBJECTSCALLBACKA) lpCallback)(&doi, pvRef) == DIENUM_STOP)
        break;
    } else {
      DIDEVICEOBJECTINSTANCEW doi;

      if(DinputVersion >= 0x0500)
        doi.dwSize = sizeof(DIDEVICEOBJECTINSTANCEW);
      else
        doi.dwSize = sizeof(DIDEVICEOBJECTINSTANCE_DX3W);

      doi.guidType = objGUID[i];
      doi.dwOfs = objOffs[i];
      doi.dwType = (thisIndex << 8) | objType[i];
      doi.dwFlags = objFlags[i];
      swprintf_s(doi.tszName, MAX_PATH, objNameW[i]);

      if(DinputVersion >= 0x0500) {
        doi.dwFFMaxForce = objFFMaxForce[i];
        doi.dwFFForceResolution = objFFResolution[i];
        doi.wCollectionNumber = thisIndex;
        doi.wDesignatorIndex = 0;
        doi.wUsagePage = (WORD) (objPageAndUsage[i] >> 16) & 0xFFFF;
        doi.wUsage = (WORD) (objPageAndUsage[i] & 0xFFFF);
        doi.dwDimension = 0;
        doi.wExponent = 0;
        doi.wReportId = 0;
      }

      if(lpCallback(&doi, pvRef) == DIENUM_STOP)
        break;
    }
  }

  return DI_OK;
}

HRESULT XDinputDevice::GetProperty(REFGUID rGuidProp, LPDIPROPHEADER diph) {
  printf("[%d] GetProperty\n", thisIndex);

  long data = ((int) diph) + diph->dwHeaderSize;
  int propertyID = (int) &rGuidProp;

  if(propertyID == (int) &DIPROP_APPDATA) {
    puts("  DIPROP_APPDATA");
    return DIERR_OBJECTNOTFOUND;
  }

  if(propertyID == (int) &DIPROP_AUTOCENTER) {
    puts("  DIPROP_AUTOCENTER");
    setPointedDWORD(&data, DIPROPAUTOCENTER_ON);
    return DI_OK;
  }

  if(propertyID == (int) &DIPROP_AXISMODE) {
    puts("  DIPROP_AXISMODE");
    setPointedDWORD(&data, DIPROPAXISMODE_ABS);
    return DI_OK;
  }

  if(propertyID == (int) &DIPROP_BUFFERSIZE) {
    puts("  DIPROP_BUFFERSIZE");
    setPointedDWORD(&data, 0);
    return DI_OK;
  }

  if(propertyID == (int) &DIPROP_CALIBRATION) {
    puts("  DIPROP_CALIBRATION");
    return DIERR_UNSUPPORTED;
  }

  if(propertyID == (int) &DIPROP_CPOINTS) {
    puts("  DIPROP_CPOINTS");
    return DIERR_UNSUPPORTED;
  }

  if(propertyID == (int) &DIPROP_DEADZONE) {
    puts("  DIPROP_DEADZONE");
    setPointedDWORD(&data, 0);
    return DI_OK;
  }

  if(propertyID == (int) &DIPROP_FFGAIN) {
    puts("  DIPROP_FFGAIN");
    setPointedDWORD(&data, 10000);
    return DI_OK;
  }

  if(propertyID == (int) &DIPROP_FFLOAD) {
    puts("  DIPROP_FFLOAD");
    setPointedDWORD(&data, 0);
    return DI_OK;
  }

  if(propertyID == (int) &DIPROP_GRANULARITY) {
    puts("  DIPROP_GRANULARITY");
    setPointedDWORD(&data, 1);
    return DI_OK;
  }

  if(propertyID == (int) &DIPROP_GUIDANDPATH) {
    puts("  DIPROP_GUIDANDPATH");
    return DIERR_UNSUPPORTED;
  }
  
  if(propertyID == (int) &DIPROP_GETPORTDISPLAYNAME || propertyID == (int) &DIPROP_INSTANCENAME) {
    if(propertyID == (int) &DIPROP_GETPORTDISPLAYNAME)
      puts("  DIPROP_GETPORTDISPLAYNAME");
    else
      puts("  DIPROP_INSTANCENAME");

    if(IsAnsi) {
      CHAR* s = (CHAR*) &data;
      sprintf_s(s, MAX_PATH, XDINPUT_INSTANCENAMEA, thisIndex);
    } else {
      WCHAR* s = (WCHAR*) &data;
      swprintf_s(s, MAX_PATH, XDINPUT_INSTANCENAMEW, thisIndex);
    }

    return DI_OK;
  }

  if(propertyID == (int) &DIPROP_JOYSTICKID) {
    puts("  DIPROP_JOYSTICKID");
    setPointedDWORD(&data, thisIndex);
    return DI_OK;
  }

  if(propertyID == (int) &DIPROP_KEYNAME) {
    puts("  DIPROP_KEYNAME");
    return DIERR_UNSUPPORTED;
  }

  if(propertyID == (int) &DIPROP_LOGICALRANGE || propertyID == (int) &DIPROP_PHYSICALRANGE) {
    if(propertyID == (int) &DIPROP_LOGICALRANGE)
      puts("  DIPROP_LOGICALRANGE");
    else
      puts("  DIPROP_PHYSICALRANGE");

    setPointedDWORD(&data, -32768);
    setPointedDWORD(&data, 32767);
    return DI_OK;
  }

  if(propertyID == (int) &DIPROP_PRODUCTNAME) {
    puts("  DIPROP_PRODUCTNAME");

    if(IsAnsi) {
      CHAR* s = (CHAR*) &data;
      sprintf_s(s, MAX_PATH, XDINPUT_INSTANCENAMEA, thisIndex + 1);
    } else {
      WCHAR* s = (WCHAR*) &data;
      swprintf_s(s, MAX_PATH, XDINPUT_INSTANCENAMEW, thisIndex + 1);
    }

    return DI_OK;
  }

  if(propertyID == (int) &DIPROP_RANGE) {
    puts("  DIPROP_RANGE");
    setPointedDWORD(&data, AxisRangeMin);
    setPointedDWORD(&data, AxisRangeMax);
    return DI_OK;
  }

  if(propertyID == (int) &DIPROP_SATURATION) {
    puts("  DIPROP_SATURATION");
    setPointedDWORD(&data, 10000);
    return DI_OK;
  }

  if(propertyID == (int) &DIPROP_SCANCODE) {
    puts("  DIPROP_SCANCODE");
    return E_INVALIDARG;
  }

  if(propertyID == (int) &DIPROP_TYPENAME) {
    puts("  DIPROP_TYPENAME");
    return DIERR_UNSUPPORTED;
  }

  if(propertyID == (int) &DIPROP_USERNAME) {
    puts("  DIPROP_USERNAME");
    return S_FALSE;
  }

  if(propertyID == (int) &DIPROP_VIDPID) {
    puts("  DIPROP_VIDPID");
    setPointedDWORD(&data, 0x69690420);
    return DI_OK;
  }

  return DIERR_INVALIDPARAM;
}

HRESULT XDinputDevice::SetProperty(REFGUID rGuidProp, LPCDIPROPHEADER diph) {
  printf("[%d] SetProperty\n", thisIndex);

  long data = ((int) diph) + diph->dwHeaderSize;
  int propertyID = (int) &rGuidProp;

  if(propertyID == (int) &DIPROP_BUFFERSIZE) {
    puts("  DIPROP_BUFFERSIZE");
    return DI_OK;
  }

  if(propertyID == (int) &DIPROP_RANGE) {
    AxisRangeMin = getPointedDWORD(&data);
    AxisRangeMax = getPointedDWORD(&data);
    printf("  DIPROP_RANGE: %d,%d\n", AxisRangeMin, AxisRangeMax);

    return DI_OK;
  }

  if(propertyID == (int) &DIPROP_AUTOCENTER) {
    puts("  DIPROP_AUTOCENTER");
    CenterAxes();

    return DI_OK;
  }
  
  puts("  other");
  return E_FAIL;
}

HRESULT XDinputDevice::Acquire(void) {
  if(IsAcquired)
    return S_FALSE;

  printf("[%d] Acquire\n", thisIndex);
  IsAcquired = TRUE;
  return DI_OK;
}

HRESULT XDinputDevice::Unacquire(void) {
  if(!IsAcquired)
    return DI_NOEFFECT;

  printf("[%d] Unacquire\n", thisIndex);
  IsAcquired = FALSE;
  return DI_OK;
}

HRESULT XDinputDevice::GetDeviceState(DWORD cbData, LPVOID lpvData) {
  printf("[%d] GetDeviceState\n", thisIndex); // too spammy
  Poll();

  float valueLX = limitValue(-1, 1, sin(state.Gamepad.sThumbLX / 32768.0f) - restingLX);
  float valueLY = limitValue(-1, 1, sin(state.Gamepad.sThumbLY / 32768.0f) - restingLY);
  float valueRX = limitValue(-1, 1, sin(state.Gamepad.sThumbRX / 32768.0f) - restingRX);
  float valueRY = limitValue(-1, 1, sin(state.Gamepad.sThumbRY / 32768.0f) - restingRY);
  float valueLT = limitValue( 0, 1, sin(state.Gamepad.bLeftTrigger / 255.0f) - restingLT);
  float valueRT = limitValue( 0, 1, sin(state.Gamepad.bRightTrigger / 255.0f) - restingRT);
  LONG lx, ly, rx, ry, lt, rt;
  DWORD pov;
  BYTE btn[32] = {0};
  int i;

  // Map sticks to the caller's desired ranges
  if(state.Gamepad.sThumbLX < 0)
    lx = (LONG) lerp(0, AxisRangeMin, -valueLX);
  else
    lx = (LONG) lerp(0, AxisRangeMax, valueLX);

  if(state.Gamepad.sThumbLY < 0)
    ly = (LONG) lerp(0, AxisRangeMax, -valueLY);
  else
    ly = (LONG) lerp(0, AxisRangeMin, valueLY);

  if(state.Gamepad.sThumbRX < 0)
    rx = (LONG) lerp(0, AxisRangeMin, -valueRX);
  else
    rx = (LONG) lerp(0, AxisRangeMax, valueRX);

  if(state.Gamepad.sThumbRY < 0)
    ry = (LONG) lerp(0, AxisRangeMax, -valueRY);
  else
    ry = (LONG) lerp(0, AxisRangeMin, valueRY);

  // ...same for triggers
  lt = (LONG) lerp(0, AxisRangeMax, valueLT);
  rt = (LONG) lerp(0, AxisRangeMax, valueRT);

  // Set POV direction
  switch(state.Gamepad.wButtons & 15) {
    case (XINPUT_GAMEPAD_DPAD_UP | XINPUT_GAMEPAD_DPAD_RIGHT):
      pov = 4500;
      break;

    case (XINPUT_GAMEPAD_DPAD_RIGHT | XINPUT_GAMEPAD_DPAD_DOWN):
      pov = 13500;
      break;

    case (XINPUT_GAMEPAD_DPAD_DOWN | XINPUT_GAMEPAD_DPAD_LEFT):
      pov = 22500;
      break;

    case (XINPUT_GAMEPAD_DPAD_LEFT | XINPUT_GAMEPAD_DPAD_UP):
      pov = 31500;
      break;

    case XINPUT_GAMEPAD_DPAD_UP:
      pov = 0;
      break;

    case XINPUT_GAMEPAD_DPAD_RIGHT:
      pov = 9000;
      break;

    case XINPUT_GAMEPAD_DPAD_DOWN:
      pov = 18000;
      break;

    case XINPUT_GAMEPAD_DPAD_LEFT:
      pov = 27000;
      break;

    default:
      pov = -1;
      break;
  }

  // Fill in pressed buttons
  if(state.Gamepad.wButtons & XINPUT_GAMEPAD_A)
    btn[0] = 128;
  if(state.Gamepad.wButtons & XINPUT_GAMEPAD_B)
    btn[1] = 128;
  if(state.Gamepad.wButtons & XINPUT_GAMEPAD_X)
    btn[2] = 128;
  if(state.Gamepad.wButtons & XINPUT_GAMEPAD_Y)
    btn[3] = 128;
  if(state.Gamepad.wButtons & XINPUT_GAMEPAD_START)
    btn[4] = 128;
  if(state.Gamepad.wButtons & XINPUT_GAMEPAD_BACK)
    btn[5] = 128;
  if(state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER)
    btn[6] = 128;
  if(state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER)
    btn[7] = 128;
  if(state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB)
    btn[8] = 128;
  if(state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB)
    btn[9] = 128;

  // Map triggers to buttons 11 & 12
  if(valueLT >= 0.8f)
    btn[10] = 128;

  if(valueRT >= 0.8f)
    btn[11] = 128;

  IsPolled = FALSE;

  // Copy results manually because array pointers screw this up
  if(dataFormat == 1) {
    LPDIJOYSTATE d = (LPDIJOYSTATE) lpvData;

    d->lX = lx;
    d->lY = ly;
    d->lRx = rx;
    d->lRy = ry;
    d->rglSlider[0] = lt;
    d->rglSlider[1] = rt;
    d->rgdwPOV[0] = pov;

    for(i = 0; i < 32; i++)
      d->rgbButtons[i] = (i < 12 ? btn[i] : 0);

    d->lZ = d->lRz = 0;
    return DI_OK;
  }

  if(dataFormat == 2) {
    LPDIJOYSTATE2 d = (LPDIJOYSTATE2) lpvData;

    d->lX = lx;
    d->lY = ly;
    d->lRx = rx;
    d->lRy = ry;
    d->rglSlider[0] = lt;
    d->rglSlider[1] = rt;
    d->rgdwPOV[0] = pov;

    for(i = 0; i < 128; i++)
      d->rgbButtons[i] = (i < 12 ? btn[i] : 0);
    
    d->lZ = d->lRz = 0;
    d->lVX = d->lVY = d->lVZ = d->lVRx = d->lVRy = d->lVRz = 0;
    d->lAX = d->lAY = d->lAZ = d->lARx = d->lARy = d->lFRz = 0;
    d->lFX = d->lFY = d->lFZ = d->lFRx = d->lFRy = d->lFRz = 0;
    d->rglVSlider[0] = d->rglASlider[0] = d->rglFSlider[0] = 0;
    d->rgdwPOV[1] = d->rgdwPOV[2] = d->rgdwPOV[3] = 0;

    return DI_OK;
  }

  return E_FAIL;
}

HRESULT XDinputDevice::GetDeviceData(DWORD, LPDIDEVICEOBJECTDATA, LPDWORD, DWORD) {
  printf("[%d] GetDeviceData\n", thisIndex);
  return DIERR_NOTBUFFERED;
}

HRESULT XDinputDevice::SetDataFormat(LPCDIDATAFORMAT lpFormat) {
  printf("[%d] SetDataFormat\n", thisIndex);

  if(lpFormat->dwSize != sizeof(DIDATAFORMAT))
    return DIERR_NOTINITIALIZED;

  absoluteMode = (lpFormat->dwFlags == DIDF_ABSAXIS);

  if(
    lpFormat->dwSize == (c_dfDIJoystick.dwSize) &&
    lpFormat->dwDataSize == (c_dfDIJoystick.dwDataSize) &&
    lpFormat->dwObjSize == (c_dfDIJoystick.dwObjSize) &&
    lpFormat->dwNumObjs == (c_dfDIJoystick.dwNumObjs) &&
    lpFormat->dwFlags == (c_dfDIJoystick.dwFlags)
  ) {
    dataFormat = 1;
    return DI_OK;
  }
  
  if(
    lpFormat->dwSize == (c_dfDIJoystick2.dwSize) &&
    lpFormat->dwDataSize == (c_dfDIJoystick2.dwDataSize) &&
    lpFormat->dwObjSize == (c_dfDIJoystick2.dwObjSize) &&
    lpFormat->dwNumObjs == (c_dfDIJoystick2.dwNumObjs) &&
    lpFormat->dwFlags == (c_dfDIJoystick2.dwFlags)
  ) {
    dataFormat = 2;
    return DI_OK;
  }

  DWORD i;
  dataFormat = 0;
  dataFormatValues = new DIOBJECTDATAFORMAT[lpFormat->dwNumObjs];

  for(i = 0; i < lpFormat->dwNumObjs; i++)
    dataFormatValues[i] = lpFormat->rgodf[i];

  return DI_OK;
}

HRESULT XDinputDevice::SetEventNotification(HANDLE) {
  printf("[%d] SetEventNotification\n", thisIndex);
  return DI_POLLEDDEVICE;
}

HRESULT XDinputDevice::SetCooperativeLevel(HWND hwnd, DWORD dwFlags) {
  printf("[%d] SetCooperativeLevel\n", thisIndex);
  printf("  Handle: $%08X\n", hwnd);
  if(dwFlags & DISCL_EXCLUSIVE)    puts("    DISCL_EXCLUSIVE");
  if(dwFlags & DISCL_NONEXCLUSIVE) puts("    DISCL_NONEXCLUSIVE");
  if(dwFlags & DISCL_FOREGROUND)   puts("    DISCL_FOREGROUND");
  if(dwFlags & DISCL_BACKGROUND)   puts("    DISCL_BACKGROUND");
  if(dwFlags & DISCL_NOWINKEY)     puts("    DISCL_NOWINKEY");

  if(hwnd == NULL)
    return E_HANDLE;

  return DI_OK;
}

HRESULT XDinputDevice::GetObjectInfo(LPDIDEVICEOBJECTINSTANCEW pdidoi, DWORD dwObj, DWORD dwHow) {
  printf("[%d] GetObjectInfo\n", thisIndex);

  if(IsAnsi) {
    LPDIDEVICEOBJECTINSTANCEA doi = (LPDIDEVICEOBJECTINSTANCEA) pdidoi;

    if(DinputVersion >= 0x0500 && doi->dwSize != sizeof(DIDEVICEOBJECTINSTANCEA))
      return DIERR_NOTINITIALIZED;
    else
    if(DinputVersion < 0x0500 && doi->dwSize != sizeof(DIDEVICEOBJECTINSTANCE_DX3A))
      return DIERR_NOTINITIALIZED;

    switch(dwHow) {
      case DIPH_DEVICE:
        puts("  DIPH_DEVICE");
        return DIERR_OBJECTNOTFOUND;

      case DIPH_BYOFFSET: {
        int item;
        puts("  DIPH_BYOFFSET");

        for(item = 0; item < objCount; item++) {
          if(objOffs[item] != dwObj)
            continue;

          doi->guidType = objGUID[item];
          doi->dwOfs = objOffs[item];
          doi->dwType = (thisIndex << 8) | objType[item];
          doi->dwFlags = objFlags[item];
          sprintf_s(doi->tszName, MAX_PATH, objNameA[item]);

          if(DinputVersion >= 0x0500) {
            doi->dwFFMaxForce = objFFMaxForce[item];
            doi->dwFFForceResolution = objFFResolution[item];
            doi->wCollectionNumber = thisIndex;
            doi->wDesignatorIndex = 0;
            doi->wUsagePage = (WORD) (objPageAndUsage[item] >> 16);
            doi->wUsage = (WORD) (objPageAndUsage[item] & 0xFFFF);
            doi->dwDimension = 0;
            doi->wExponent = 0;
            doi->wReportId = 0;
          }

          printf("    $%X: %s\n", doi->dwOfs, doi->tszName);
          return DI_OK;
        }

        doi->guidType = GUID_NULL;
        doi->dwOfs = dwObj;
        doi->dwType = DIDFT_NODATA;
        doi->dwFlags = 0;
        sprintf_s(doi->tszName, MAX_PATH, XDOA_UNDEF);

        if(DinputVersion >= 0x0500) {
          doi->dwFFMaxForce = 0;
          doi->dwFFForceResolution = 0;
          doi->wCollectionNumber = 0;
          doi->wDesignatorIndex = 0;
          doi->wUsagePage = 0;
          doi->wUsage = 0;
          doi->dwDimension = 0;
          doi->wExponent = 0;
          doi->wReportId = 0;
        }

        printf("    $%X: Not found\n", dwObj);
        return DIERR_OBJECTNOTFOUND;
      }

      case DIPH_BYID:
        puts("  DIPH_BYID");
        return DIERR_OBJECTNOTFOUND;

      case DIPH_BYUSAGE:
        puts("  DIPH_BYUSAGE");
        return DIERR_OBJECTNOTFOUND;
    }
  } else {
    LPDIDEVICEOBJECTINSTANCEW doi = (LPDIDEVICEOBJECTINSTANCEW) pdidoi;

    if(DinputVersion >= 0x0500 && doi->dwSize != sizeof(DIDEVICEOBJECTINSTANCEW))
      return DIERR_NOTINITIALIZED;
    else
    if(DinputVersion < 0x0500 && doi->dwSize != sizeof(DIDEVICEOBJECTINSTANCE_DX3W))
      return DIERR_NOTINITIALIZED;

    switch(dwHow) {
      case DIPH_DEVICE:
        puts("  DIPH_DEVICE");
        return DIERR_OBJECTNOTFOUND;

      case DIPH_BYOFFSET: {
        int item;
        puts("  DIPH_BYOFFSET");

        for(item = 0; item < objCount; item++) {
          if(objOffs[item] != dwObj)
            continue;

          doi->guidType = objGUID[item];
          doi->dwOfs = objOffs[item];
          doi->dwType = (thisIndex << 8) | objType[item];
          doi->dwFlags = objFlags[item];
          swprintf_s(doi->tszName, MAX_PATH, objNameW[item]);

          if(DinputVersion >= 0x0500) {
            doi->dwFFMaxForce = objFFMaxForce[item];
            doi->dwFFForceResolution = objFFResolution[item];
            doi->wCollectionNumber = thisIndex;
            doi->wDesignatorIndex = 0;
            doi->wUsagePage = (WORD) (objPageAndUsage[item] >> 16);
            doi->wUsage = (WORD) (objPageAndUsage[item] & 0xFFFF);
            doi->dwDimension = 0;
            doi->wExponent = 0;
            doi->wReportId = 0;
          }

          wprintf(L"    $%X: %s\n", doi->dwOfs, doi->tszName);
          return DI_OK;
        }

        doi->guidType = GUID_NULL;
        doi->dwOfs = dwObj;
        doi->dwType = DIDFT_NODATA;
        doi->dwFlags = 0;
        swprintf_s(doi->tszName, MAX_PATH, XDOW_UNDEF);

        if(DinputVersion >= 0x0500) {
          doi->dwFFMaxForce = 0;
          doi->dwFFForceResolution = 0;
          doi->wCollectionNumber = 0;
          doi->wDesignatorIndex = 0;
          doi->wUsagePage = 0;
          doi->wUsage = 0;
          doi->dwDimension = 0;
          doi->wExponent = 0;
          doi->wReportId = 0;
        }

        printf("    $%X: Not found\n", dwObj);
        return DIERR_OBJECTNOTFOUND;
      }

      case DIPH_BYID:
        puts("  DIPH_BYID");
        return DIERR_OBJECTNOTFOUND;

      case DIPH_BYUSAGE:
        puts("  DIPH_BYUSAGE");
        return DIERR_OBJECTNOTFOUND;
    }
  }

  return DIERR_INVALIDPARAM;
}

HRESULT XDinputDevice::GetDeviceInfo(LPDIDEVICEINSTANCEW lpdi) {
  printf("[%d] GetDeviceInfo\n", thisIndex);

  if(lpdi == NULL)
    return E_POINTER;

  if(IsAnsi) {
    LPDIDEVICEINSTANCEA di = (LPDIDEVICEINSTANCEA) lpdi;

    if(di->dwSize != sizeof(DIDEVICEINSTANCEA) && di->dwSize != sizeof(DIDEVICEINSTANCE_DX3A))
      return DIERR_NOTINITIALIZED;

    XDINPUT_CONTROLLER_GUID(di->guidInstance, thisIndex);
    di->guidProduct = XDINPUT_PRODUCT_GUID;
    di->dwDevType = DIDEVTYPE_HID | DI8DEVCLASS_GAMECTRL;
    if(DinputVersion <= 0x0700) di->dwDevType |= DI8DEVTYPE_GAMEPAD;
    sprintf_s(di->tszInstanceName, MAX_PATH, XDINPUT_INSTANCENAMEA, thisIndex + 1);
    sprintf_s(di->tszProductName, MAX_PATH, XDINPUT_PRODUCTNAMEA);

    if(DinputVersion >= 0x0500) {
      di->guidFFDriver = XDINPUT_VIBRATOR_GUID;
      di->wUsagePage = 0x01; // HID Generic Desktop page
      di->wUsage = 0x05; // HID Gamepad
    }

    return DI_OK;
  }

  LPDIDEVICEINSTANCEW di = (LPDIDEVICEINSTANCEW) lpdi;

  if(di->dwSize != sizeof(DIDEVICEINSTANCEW) && di->dwSize != sizeof(DIDEVICEINSTANCE_DX3W))
    return DIERR_NOTINITIALIZED;

  XDINPUT_CONTROLLER_GUID(di->guidInstance, thisIndex);
  di->guidProduct = XDINPUT_PRODUCT_GUID;
  di->dwDevType = DIDEVTYPE_HID | DI8DEVCLASS_GAMECTRL;
  if(DinputVersion <= 0x0700) di->dwDevType |= DI8DEVTYPE_GAMEPAD;
  swprintf_s(di->tszInstanceName, MAX_PATH, XDINPUT_INSTANCENAMEW, thisIndex + 1);
  swprintf_s(di->tszProductName, MAX_PATH, XDINPUT_PRODUCTNAMEW);

  if(DinputVersion >= 0x0500) {
    di->guidFFDriver = XDINPUT_VIBRATOR_GUID;
    di->wUsage = di->wUsagePage = 0;
  }

  return DI_OK;
}

HRESULT XDinputDevice::RunControlPanel(HWND, DWORD) {
  printf("[%d] RunControlPanel\n", thisIndex);
  return E_FAIL;
}

HRESULT XDinputDevice::Initialize(HINSTANCE, DWORD, REFGUID) {
  printf("[%d] Initialize\n", thisIndex);
  return DI_OK;
}

HRESULT XDinputDevice::CreateEffect(REFGUID rGuid, LPCDIEFFECT lpEff, LPDIRECTINPUTEFFECT *lpDeff, LPUNKNOWN pUnkOuter) {
  printf("[%d] CreateEffect\n", thisIndex);
  if(effectsUsed == MAX_EFFECTS)
    return DIERR_DEVICEFULL;

  if(rGuid == GUID_ConstantForce) {
    *lpDeff = thisEffect[effectsUsed++] = new XDinputConstantForceEffect(thisIndex);
    return DI_OK;
  }

//  *lpDeff = thisEffect[effectsUsed++] = new XDinputEffect(rGuid, "NullEffect", L"NullEffect");
  return DIERR_INVALIDPARAM;
}

HRESULT XDinputDevice::EnumEffects(LPDIENUMEFFECTSCALLBACKW lpCallback, LPVOID pvRef, DWORD dwEffType) {
  printf("[%d] EnumEffects\n", thisIndex);

  if(dwEffType > DIEFT_CONSTANTFORCE)
    return DI_OK;

  if(IsAnsi) {
    DIEFFECTINFOA ei;
    ei.dwSize = sizeof(DIEFFECTINFOA);
    ei.guid = GUID_ConstantForce;
    ei.dwEffType = DIEFT_CONSTANTFORCE;
    ei.dwStaticParams = DIEP_TYPESPECIFICPARAMS;
    ei.dwDynamicParams = DIEP_DIRECTION;
    sprintf_s(ei.tszName, MAX_PATH, XDINPUT_EFFECTNAMEA);

    ((LPDIENUMEFFECTSCALLBACKA) lpCallback)(&ei, pvRef);
  } else {
    DIEFFECTINFOW ei;
    ei.dwSize = sizeof(DIEFFECTINFOA);
    ei.guid = GUID_ConstantForce;
    ei.dwEffType = DIEFT_CONSTANTFORCE;
    ei.dwStaticParams = DIEP_TYPESPECIFICPARAMS;
    ei.dwDynamicParams = DIEP_DIRECTION;
    swprintf_s(ei.tszName, MAX_PATH, XDINPUT_EFFECTNAMEW);

    ((LPDIENUMEFFECTSCALLBACKW) lpCallback)(&ei, pvRef);
  }

  return DI_OK;
}

HRESULT XDinputDevice::GetEffectInfo(LPDIEFFECTINFOW, REFGUID) {
  printf("[%d] GetEffectInfo\n", thisIndex);
  return E_FAIL;
}

HRESULT XDinputDevice::GetForceFeedbackState(LPDWORD) {
  printf("[%d] GetForceFeedbackState\n", thisIndex);
  return E_FAIL;
}

HRESULT XDinputDevice::SendForceFeedbackCommand(DWORD dwFlags) {
  printf("[%d] SendForceFeedbackCommand\n", thisIndex);
  int i;

  if(dwFlags & (DISFFC_RESET | DISFFC_STOPALL | DISFFC_PAUSE | DISFFC_SETACTUATORSOFF))
    for(i = 0; i < effectsUsed; i++)
      thisEffect[i]->Stop();

  if(dwFlags & DISFFC_RESET) {
    for(i = 0; i < effectsUsed; i++) {
      delete thisEffect[i];
      thisEffect[i] = NULL;
    }

    effectsUsed = 0;
    return (i == 0 ? DI_NOEFFECT : DI_OK);
  }

  if(dwFlags & DISFFC_SETACTUATORSON)
    for(i = 0; i < effectsUsed; i++)
      thisEffect[i]->Start(0, 0);
  else
  if(dwFlags & DISFFC_SETACTUATORSOFF)
    for(i = 0; i < effectsUsed; i++)
      thisEffect[i]->Enabled = FALSE;

  return (i == 0 ? DI_NOEFFECT : DI_OK);
}

HRESULT XDinputDevice::EnumCreatedEffectObjects(LPDIENUMCREATEDEFFECTOBJECTSCALLBACK lpCallback, LPVOID pvRef, DWORD) {
  printf("[%d] EnumCreatedEffectObjects\n", thisIndex);

  for(int i = 0; i < effectsUsed; i++)
    if(lpCallback(thisEffect[i], pvRef) == DIENUM_STOP)
      break;

  return DI_OK;
}

HRESULT XDinputDevice::Escape(LPDIEFFESCAPE) {
  printf("[%d] Escape\n", thisIndex);
  return DIERR_READONLY;
}

HRESULT XDinputDevice::Poll(void) {
  if(IsPolled)
    return DI_OK;

  IsPolled = TRUE;
  return (XInputGetState(thisIndex, &state) == ERROR_DEVICE_NOT_CONNECTED ? DIERR_INPUTLOST : DI_OK);
}

HRESULT XDinputDevice::SendDeviceData(DWORD, LPCDIDEVICEOBJECTDATA, LPDWORD, DWORD) {
  printf("[%d] SendDeviceData\n", thisIndex);
  return DIERR_UNSUPPORTED;
}

HRESULT XDinputDevice::EnumEffectsInFile(LPCWSTR, LPDIENUMEFFECTSINFILECALLBACK, LPVOID, DWORD) {
  printf("[%d] EnumEffectsInFile\n", thisIndex);
  return E_FAIL;
}

HRESULT XDinputDevice::WriteEffectToFile(LPCWSTR, DWORD, LPDIFILEEFFECT, DWORD) {
  printf("[%d] WriteEffectToFile\n", thisIndex);
  return E_FAIL;
}

HRESULT XDinputDevice::BuildActionMap(LPDIACTIONFORMATW, LPCWSTR, DWORD) {
  printf("[%d] BuildActionMap\n", thisIndex);
  return E_FAIL;
}

HRESULT XDinputDevice::SetActionMap(LPDIACTIONFORMATW, LPCWSTR, DWORD) {
  printf("[%d] SetActionMap\n", thisIndex);
  return E_FAIL;
}

HRESULT XDinputDevice::GetImageInfo(LPDIDEVICEIMAGEINFOHEADERW) {
  printf("[%d] GetImageInfo\n", thisIndex);
  return E_FAIL;
}
