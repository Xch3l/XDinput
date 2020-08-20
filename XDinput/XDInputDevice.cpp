#include "StdAfx.h"
#include "XDInputDevice.h"
#include "XDinputEffect.h"
#include <XInput.h>

XDinputDevice::XDinputDevice(int iIndex, BOOL isAnsi) {
  ZeroMemory(&state, sizeof(XINPUT_STATE));
  thisIndex = iIndex;
  IsAcquired = FALSE;
  IsAnsi = isAnsi;
  absoluteMode = TRUE;

  dataFormatValues = NULL;
  dataFormat = 0;

  AxisRangeMin = -65536;
  AxisRangeMax = 65536;
}

XDinputDevice::~XDinputDevice(void) {
}

///////////////////////////////////////////////////

const int objCount = 21;
const CHAR* objNameA[] = {
  "Left Thumb Y", "Left Thumb X", "Right Thumb Y", "Right Thumb X",  "Left Trigger",      "Right Trigger",
  "A",            "B",            "X",             "Y",              "Start",             "Back",
  "Left Thumb",   "Right Thumb",  "Left Shoulder", "Right Shoulder", "Left Trigger Down", "Right Trigger Down",
  "D-Pad",        "LoFreq Motor", "HiFreq Motor"
};
const WCHAR* objNameW[] = {
  L"Left Thumb Y", L"Left Thumb X", L"Right Thumb Y", L"Right Thumb X",  L"Left Trigger",      L"Right Trigger",
  L"A",            L"B",            L"X",             L"Y",              L"Start",             L"Back",
  L"Left Thumb",   L"Right Thumb",  L"Left Shoulder", L"Right Shoulder", L"Left Trigger Down", L"Right Trigger Down",
  L"D-Pad",        L"LoFreq Motor", L"HiFreq Motor"
};
const GUID objGUID[] = {
  GUID_YAxis,  GUID_XAxis,  GUID_RyAxis, GUID_RxAxis, GUID_Slider, GUID_Slider,
  GUID_Button, GUID_Button, GUID_Button, GUID_Button, GUID_Button, GUID_Button,
  GUID_Button, GUID_Button, GUID_Button, GUID_Button, GUID_Button, GUID_Button,
  GUID_POV, XDINPUT_VIBRATOR_GUID, XDINPUT_VIBRATOR_GUID
};
const DWORD objType[] = {
  DIDFT_ABSAXIS, DIDFT_ABSAXIS, DIDFT_ABSAXIS, DIDFT_ABSAXIS, DIDFT_ABSAXIS, DIDFT_ABSAXIS,
  DIDFT_PSHBUTTON, DIDFT_PSHBUTTON, DIDFT_PSHBUTTON, DIDFT_PSHBUTTON, DIDFT_PSHBUTTON, DIDFT_PSHBUTTON,
  DIDFT_PSHBUTTON, DIDFT_PSHBUTTON, DIDFT_PSHBUTTON, DIDFT_PSHBUTTON, DIDFT_PSHBUTTON, DIDFT_PSHBUTTON,
  DIDFT_POV, DIDFT_FFACTUATOR, DIDFT_FFACTUATOR
};
const DWORD objFlags[] = {
  DIDOI_ASPECTPOSITION, DIDOI_ASPECTPOSITION, DIDOI_ASPECTPOSITION, DIDOI_ASPECTPOSITION, DIDOI_ASPECTPOSITION, DIDOI_ASPECTPOSITION,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, DIDOI_FFACTUATOR, DIDOI_FFACTUATOR
};
const DWORD objFFMaxForce[] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 65535, 65535
};
const DWORD objFFResolution[] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1
};
const WORD objUsage[] = {
  49, 48, 52, 51, 50, 49, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 57, 64, 65
};

///////////////////////////////////////////////////
/// IUnknown implements
ULONG WINAPI XDinputDevice::AddRef() {
  return 1;
}

HRESULT WINAPI XDinputDevice::QueryInterface(REFIID riid, LPVOID *ppvObject) {
  *ppvObject = NULL;
  return E_NOINTERFACE;
}

ULONG WINAPI XDinputDevice::Release() {
  delete this;
  return 0;
}

///////////////////////////////////////////////////
/// IDirectInputDevice8 implements
HRESULT XDinputDevice::GetCapabilities(LPDIDEVCAPS lpDIDevCaps) {
  XINPUT_CAPABILITIES caps;
  int status;

  ZeroMemory(&caps, sizeof(XINPUT_CAPABILITIES));
  status = XInputGetCapabilities(thisIndex, XINPUT_FLAG_GAMEPAD, &caps);

  lpDIDevCaps->dwFlags = (
    DIDC_EMULATED | DIDC_POLLEDDATAFORMAT | DIDC_POLLEDDEVICE |
    DIDC_FORCEFEEDBACK | DIDC_FFFADE | DIDC_FFATTACK | DIDC_POSNEGCOEFFICIENTS | DIDC_POSNEGSATURATION | DIDC_SATURATION | DIDC_STARTDELAY
  );

  if(status == ERROR_SUCCESS)
    lpDIDevCaps->dwFlags |= DIDC_ATTACHED;

  lpDIDevCaps->dwDevType = DI8DEVTYPE_GAMEPAD;
  lpDIDevCaps->dwAxes = 4;
  lpDIDevCaps->dwButtons = 12;
  lpDIDevCaps->dwPOVs = 1;
  lpDIDevCaps->dwFFSamplePeriod = 10;
  lpDIDevCaps->dwFFMinTimeResolution = 10;
  lpDIDevCaps->dwFirmwareRevision = 0x12345678;
  lpDIDevCaps->dwHardwareRevision = 0x12345678;
  lpDIDevCaps->dwFFDriverVersion = 0x12345678;

  return DI_OK;
}

HRESULT XDinputDevice::EnumObjects(LPDIENUMDEVICEOBJECTSCALLBACKW lpCallback, LPVOID pvRef, DWORD dwFlags) {
  for(int i = 0; i < objCount; i++) {
    if(IsAnsi) {
      DIDEVICEOBJECTINSTANCEA doi;
      doi.dwSize = sizeof(DIDEVICEOBJECTINSTANCEA);
      doi.dwOfs = (i * 4);

      sprintf_s(doi.tszName, MAX_PATH, objNameA[i]);
      doi.guidType = objGUID[i];
      doi.dwType = (thisIndex << 8) | objType[i];
      doi.dwFlags = objFlags[i];
      doi.dwFFMaxForce = objFFMaxForce[i];
      doi.dwFFForceResolution = objFFResolution[i];
      doi.wCollectionNumber = thisIndex;
      doi.wDesignatorIndex = 0;
      doi.wUsagePage = 1;
      doi.wUsage = objUsage[i];
      doi.dwDimension = 0;
      doi.wExponent = 0;
      doi.wReportId = 0;

      if(((LPDIENUMDEVICEOBJECTSCALLBACKA) lpCallback)(&doi, pvRef) == DIENUM_STOP)
        break;
    } else {
      DIDEVICEOBJECTINSTANCE doi;
      doi.dwSize = sizeof(DIDEVICEOBJECTINSTANCE);
      doi.dwOfs = (i * 4);

      swprintf_s(doi.tszName, MAX_PATH, objNameW[i]);
      doi.guidType = objGUID[i];
      doi.dwType = (thisIndex << 8) | objType[i];
      doi.dwFlags = objFlags[i];
      doi.dwFFMaxForce = objFFMaxForce[i];
      doi.dwFFForceResolution = objFFResolution[i];
      doi.wCollectionNumber = thisIndex;
      doi.wDesignatorIndex = 0;
      doi.wUsagePage = 1;
      doi.wUsage = objUsage[i];
      doi.dwDimension = 0;
      doi.wExponent = 0;
      doi.wReportId = 0;

      if(lpCallback(&doi, pvRef) == DIENUM_STOP)
        break;
    }
  }

  return DI_OK;
}

HRESULT XDinputDevice::GetProperty(REFGUID, LPDIPROPHEADER) {
  return E_FAIL;
}

HRESULT XDinputDevice::SetProperty(REFGUID rGuidProp, LPCDIPROPHEADER diph) {
  int propertyID = (int) &rGuidProp;
  int data = ((int) diph) + diph->dwHeaderSize;

  if(propertyID == 1) // DIPROP_BUFFERSIZE
    return DI_OK;

  if(propertyID == 4) { // DIPROP_RANGE
    AxisRangeMin = getPointedInt(&data);
    AxisRangeMax = getPointedInt(&data);

    return DI_OK;
  }

  if(propertyID == 9) // DIPROP_AUTOCENTER
    return DI_OK; // safe to ignore

  return E_FAIL;
}

HRESULT XDinputDevice::Acquire(void) {
  IsAcquired = TRUE;
  return DI_OK;
}

HRESULT XDinputDevice::Unacquire(void) {
  IsAcquired = FALSE;
  return DI_OK;
}

HRESULT XDinputDevice::GetDeviceState(DWORD cbData, LPVOID lpvData) {
  if(dataFormat == -100) {
    DIJOYSTATE s;
    ZeroMemory(&s, sizeof(DIJOYSTATE));

    // Map sticks to the caller's desired ranges
    if(state.Gamepad.sThumbLX < 0)
      s.lX = (LONG) lerp(0, AxisRangeMin, -(state.Gamepad.sThumbLX / 32768.0f));
    else
      s.lX = (LONG) lerp(0, AxisRangeMax, state.Gamepad.sThumbLX / 32767.0f);

    if(state.Gamepad.sThumbLY < 0)
      s.lY = (LONG) lerp(0, AxisRangeMax, -(state.Gamepad.sThumbLY / 32768.0f));
    else
      s.lY = (LONG) lerp(0, AxisRangeMin, state.Gamepad.sThumbLY / 32767.0f);

    if(state.Gamepad.sThumbRX < 0)
      s.lRx = (LONG) lerp(0, AxisRangeMin, -(state.Gamepad.sThumbRX / 32768.0f));
    else
      s.lRx = (LONG) lerp(0, AxisRangeMax, state.Gamepad.sThumbRX / 32767.0f);

    if(state.Gamepad.sThumbRY < 0)
      s.lRy = (LONG) lerp(0, AxisRangeMax, -(state.Gamepad.sThumbRY / 32768.0f));
    else
      s.lRy = (LONG) lerp(0, AxisRangeMin, state.Gamepad.sThumbRY / 32767.0f);

    // ...same for triggers
    s.rglSlider[0] = (LONG) lerp(0, AxisRangeMax, state.Gamepad.bLeftTrigger / 255.0f);
    s.rglSlider[1] =  (LONG) lerp(0, AxisRangeMax, state.Gamepad.bRightTrigger / 255.0f);

    // Set POV direction
    switch(state.Gamepad.wButtons & 15) {
      case (XINPUT_GAMEPAD_DPAD_UP | XINPUT_GAMEPAD_DPAD_RIGHT):
        s.rgdwPOV[0] = 4500;
        break;

      case (XINPUT_GAMEPAD_DPAD_RIGHT | XINPUT_GAMEPAD_DPAD_DOWN):
        s.rgdwPOV[0] = 13500;
        break;

      case (XINPUT_GAMEPAD_DPAD_DOWN | XINPUT_GAMEPAD_DPAD_LEFT):
        s.rgdwPOV[0] = 22500;
        break;

      case (XINPUT_GAMEPAD_DPAD_LEFT | XINPUT_GAMEPAD_DPAD_UP):
        s.rgdwPOV[0] = 31500;
        break;

      case XINPUT_GAMEPAD_DPAD_UP:
        s.rgdwPOV[0] = 0;
        break;

      case XINPUT_GAMEPAD_DPAD_RIGHT:
        s.rgdwPOV[0] = 9000;
        break;

      case XINPUT_GAMEPAD_DPAD_DOWN:
        s.rgdwPOV[0] = 18000;
        break;

      case XINPUT_GAMEPAD_DPAD_LEFT:
        s.rgdwPOV[0] = 27000;
        break;

      default:
        s.rgdwPOV[0] = -1;
        break;
    }

    // Fill in pressed buttons
    if(state.Gamepad.wButtons & XINPUT_GAMEPAD_A)
      s.rgbButtons[0] = 128;

    if(state.Gamepad.wButtons & XINPUT_GAMEPAD_B)
      s.rgbButtons[1] = 128;

    if(state.Gamepad.wButtons & XINPUT_GAMEPAD_X)
      s.rgbButtons[2] = 128;

    if(state.Gamepad.wButtons & XINPUT_GAMEPAD_Y)
      s.rgbButtons[3] = 128;

    if(state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER)
      s.rgbButtons[4] = 128;

    if(state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER)
      s.rgbButtons[5] = 128;

    if(state.Gamepad.wButtons & XINPUT_GAMEPAD_BACK)
      s.rgbButtons[6] = 128;

    if(state.Gamepad.wButtons & XINPUT_GAMEPAD_START)
      s.rgbButtons[7] = 128;

    if(state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB)
      s.rgbButtons[8] = 128;

    if(state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB)
      s.rgbButtons[9] = 128;

    // Map triggers to buttons
    if(state.Gamepad.bLeftTrigger >= 0xC0)
      s.rgbButtons[10] = 128;

    if(state.Gamepad.bRightTrigger >= 0xC0)
      s.rgbButtons[11] = 128;

    // Send out our result
    memcpy(lpvData, &s, sizeof(DIJOYSTATE));
    return DI_OK;

    return DI_OK;
  }

  if(dataFormat == -101) {
    DIJOYSTATE2 s;
    ZeroMemory(&s, sizeof(DIJOYSTATE2));

    // Map sticks to the caller's desired ranges
    if(state.Gamepad.sThumbLX < 0)
      s.lX = (LONG) lerp(0, AxisRangeMin, -(state.Gamepad.sThumbLX / 32768.0f));
    else
      s.lX = (LONG) lerp(0, AxisRangeMax, state.Gamepad.sThumbLX / 32767.0f);

    if(state.Gamepad.sThumbLY < 0)
      s.lY = (LONG) lerp(0, AxisRangeMax, -(state.Gamepad.sThumbLY / 32768.0f));
    else
      s.lY = (LONG) lerp(0, AxisRangeMin, state.Gamepad.sThumbLY / 32767.0f);

    if(state.Gamepad.sThumbRX < 0)
      s.lRx = (LONG) lerp(0, AxisRangeMin, -(state.Gamepad.sThumbRX / 32768.0f));
    else
      s.lRx = (LONG) lerp(0, AxisRangeMax, state.Gamepad.sThumbRX / 32767.0f);

    if(state.Gamepad.sThumbRY < 0)
      s.lRy = (LONG) lerp(0, AxisRangeMax, -(state.Gamepad.sThumbRY / 32768.0f));
    else
      s.lRy = (LONG) lerp(0, AxisRangeMin, state.Gamepad.sThumbRY / 32767.0f);

    // ...same for triggers
    s.rglSlider[0] = (LONG) lerp(0, AxisRangeMax, state.Gamepad.bLeftTrigger / 255.0f);
    s.rglSlider[1] =  (LONG) lerp(0, AxisRangeMax, state.Gamepad.bRightTrigger / 255.0f);

    // Set POV direction
    switch(state.Gamepad.wButtons & 15) {
      case (XINPUT_GAMEPAD_DPAD_UP | XINPUT_GAMEPAD_DPAD_RIGHT):
        s.rgdwPOV[0] = 4500;
        break;

      case (XINPUT_GAMEPAD_DPAD_RIGHT | XINPUT_GAMEPAD_DPAD_DOWN):
        s.rgdwPOV[0] = 13500;
        break;

      case (XINPUT_GAMEPAD_DPAD_DOWN | XINPUT_GAMEPAD_DPAD_LEFT):
        s.rgdwPOV[0] = 22500;
        break;

      case (XINPUT_GAMEPAD_DPAD_LEFT | XINPUT_GAMEPAD_DPAD_UP):
        s.rgdwPOV[0] = 31500;
        break;

      case XINPUT_GAMEPAD_DPAD_UP:
        s.rgdwPOV[0] = 0;
        break;

      case XINPUT_GAMEPAD_DPAD_RIGHT:
        s.rgdwPOV[0] = 9000;
        break;

      case XINPUT_GAMEPAD_DPAD_DOWN:
        s.rgdwPOV[0] = 18000;
        break;

      case XINPUT_GAMEPAD_DPAD_LEFT:
        s.rgdwPOV[0] = 27000;
        break;

      default:
        s.rgdwPOV[0] = -1;
        break;
    }

    // Fill in pressed buttons
    if(state.Gamepad.wButtons & XINPUT_GAMEPAD_A)
      s.rgbButtons[0] = 128;

    if(state.Gamepad.wButtons & XINPUT_GAMEPAD_B)
      s.rgbButtons[1] = 128;

    if(state.Gamepad.wButtons & XINPUT_GAMEPAD_X)
      s.rgbButtons[2] = 128;

    if(state.Gamepad.wButtons & XINPUT_GAMEPAD_Y)
      s.rgbButtons[3] = 128;

    if(state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER)
      s.rgbButtons[4] = 128;

    if(state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER)
      s.rgbButtons[5] = 128;

    if(state.Gamepad.wButtons & XINPUT_GAMEPAD_BACK)
      s.rgbButtons[6] = 128;

    if(state.Gamepad.wButtons & XINPUT_GAMEPAD_START)
      s.rgbButtons[7] = 128;

    if(state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB)
      s.rgbButtons[8] = 128;

    if(state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB)
      s.rgbButtons[9] = 128;

    // Map triggers to buttons
    if(state.Gamepad.bLeftTrigger >= 0xC0)
      s.rgbButtons[10] = 128;

    if(state.Gamepad.bRightTrigger >= 0xC0)
      s.rgbButtons[11] = 128;

    // Send out our result
    memcpy(lpvData, &s, sizeof(DIJOYSTATE2));
    return DI_OK;
  }

  return E_FAIL;
}

HRESULT XDinputDevice::GetDeviceData(DWORD, LPDIDEVICEOBJECTDATA, LPDWORD, DWORD) {
  return E_FAIL;
}

HRESULT XDinputDevice::SetDataFormat(LPCDIDATAFORMAT lpFormat) {
  if(lpFormat->dwSize != sizeof(DIDATAFORMAT))
    return DIERR_GENERIC;

  absoluteMode = (lpFormat->dwFlags == DIDF_ABSAXIS);

  if(
    lpFormat->dwSize == (c_dfDIJoystick.dwSize) &&
    lpFormat->dwDataSize == (c_dfDIJoystick.dwDataSize) &&
    lpFormat->dwObjSize == (c_dfDIJoystick.dwObjSize) &&
    lpFormat->dwNumObjs == (c_dfDIJoystick.dwNumObjs) &&
    lpFormat->dwFlags == (c_dfDIJoystick.dwFlags)
  ) {
    dataFormat = -100;
    return DI_OK;
  }
  
  if(
    lpFormat->dwSize == (c_dfDIJoystick2.dwSize) &&
    lpFormat->dwDataSize == (c_dfDIJoystick2.dwDataSize) &&
    lpFormat->dwObjSize == (c_dfDIJoystick2.dwObjSize) &&
    lpFormat->dwNumObjs == (c_dfDIJoystick2.dwNumObjs) &&
    lpFormat->dwFlags == (c_dfDIJoystick2.dwFlags)
  ) {
    dataFormat = -101;
    return DI_OK;
  }

  DWORD i;
  dataFormatValues = new DIOBJECTDATAFORMAT[lpFormat->dwNumObjs];

  for(i = 0; i < lpFormat->dwNumObjs; i++)
    dataFormatValues[i] = lpFormat->rgodf[i];

  return DI_OK;
}

HRESULT XDinputDevice::SetEventNotification(HANDLE) {
  return E_FAIL;
}

HRESULT XDinputDevice::SetCooperativeLevel(HWND, DWORD) {
  return DI_OK;
}

HRESULT XDinputDevice::GetObjectInfo(LPDIDEVICEOBJECTINSTANCEW, DWORD, DWORD) {
  return E_FAIL;
}

HRESULT XDinputDevice::GetDeviceInfo(LPDIDEVICEINSTANCEW) {
  return E_FAIL;
}

HRESULT XDinputDevice::RunControlPanel(HWND, DWORD) {
  return E_FAIL;
}

HRESULT XDinputDevice::Initialize(HINSTANCE, DWORD, REFGUID) {
  return E_FAIL;
}

HRESULT XDinputDevice::CreateEffect(REFGUID rGuid, LPCDIEFFECT lpEff, LPDIRECTINPUTEFFECT *lpDeff, LPUNKNOWN pUnkOuter) {
  if(rGuid == GUID_ConstantForce) {
    *lpDeff = new XDinputEffect(thisIndex);
    return DI_OK;
  }

  return E_FAIL;
}

HRESULT XDinputDevice::EnumEffects(LPDIENUMEFFECTSCALLBACKW, LPVOID, DWORD) {
  return E_FAIL;
}

HRESULT XDinputDevice::GetEffectInfo(LPDIEFFECTINFOW, REFGUID) {
  return E_FAIL;
}

HRESULT XDinputDevice::GetForceFeedbackState(LPDWORD) {
  return E_FAIL;
}

HRESULT XDinputDevice::SendForceFeedbackCommand(DWORD) {
  return E_FAIL;
}

HRESULT XDinputDevice::EnumCreatedEffectObjects(LPDIENUMCREATEDEFFECTOBJECTSCALLBACK, LPVOID, DWORD) {
  return E_FAIL;
}

HRESULT XDinputDevice::Escape(LPDIEFFESCAPE) {
  return E_FAIL;
}

HRESULT XDinputDevice::Poll(void) {
  return (XInputGetState(thisIndex, &state) == ERROR_DEVICE_NOT_CONNECTED ? DIERR_INPUTLOST : DI_OK);
}

HRESULT XDinputDevice::SendDeviceData(DWORD, LPCDIDEVICEOBJECTDATA, LPDWORD, DWORD) {
  return E_FAIL;
}

HRESULT XDinputDevice::EnumEffectsInFile(LPCWSTR, LPDIENUMEFFECTSINFILECALLBACK, LPVOID, DWORD) {
  return E_FAIL;
}

HRESULT XDinputDevice::WriteEffectToFile(LPCWSTR, DWORD, LPDIFILEEFFECT, DWORD) {
  return E_FAIL;
}

HRESULT XDinputDevice::BuildActionMap(LPDIACTIONFORMATW, LPCWSTR, DWORD) {
  return E_FAIL;
}

HRESULT XDinputDevice::SetActionMap(LPDIACTIONFORMATW, LPCWSTR, DWORD) {
  return E_FAIL;
}

HRESULT XDinputDevice::GetImageInfo(LPDIDEVICEIMAGEINFOHEADERW) {
  return E_FAIL;
}

///////////////////////////////////////////////////

XDinputDeviceLegacy::XDinputDeviceLegacy(int index, BOOL isAnsi) : XDinputDevice(index, isAnsi) {}

XDinputDeviceLegacy::~XDinputDeviceLegacy(void) {}

///////////////////////////////////////////////////
/// IUnknown implements
ULONG WINAPI XDinputDeviceLegacy::AddRef() {
  return XDinputDevice::AddRef();
}

HRESULT WINAPI XDinputDeviceLegacy::QueryInterface(REFIID riid, LPVOID *ppvObject) {
  return XDinputDevice::QueryInterface(riid, ppvObject);
}

ULONG WINAPI XDinputDeviceLegacy::Release() {
  return XDinputDevice::Release();
}

///////////////////////////////////////////////////
/// IDirectInputDevice implements
HRESULT WINAPI XDinputDeviceLegacy::GetCapabilities(LPDIDEVCAPS lpDIDevCaps) {
  return XDinputDevice::GetCapabilities(lpDIDevCaps);
}

HRESULT WINAPI XDinputDeviceLegacy::EnumObjects(LPDIENUMDEVICEOBJECTSCALLBACKW lpCallback, LPVOID pvRef, DWORD dwFlags) {
  return XDinputDevice::EnumObjects(lpCallback, pvRef, dwFlags);
}

HRESULT WINAPI XDinputDeviceLegacy::GetProperty(REFGUID riid, LPDIPROPHEADER lpdiPropHeader) {
  return XDinputDevice::GetProperty(riid, lpdiPropHeader);
}

HRESULT WINAPI XDinputDeviceLegacy::SetProperty(REFGUID rGuidProp, LPCDIPROPHEADER pdiph) {
  return XDinputDevice::SetProperty(rGuidProp, pdiph);
}

HRESULT WINAPI XDinputDeviceLegacy::Acquire(void) {
  return XDinputDevice::Acquire();
}

HRESULT WINAPI XDinputDeviceLegacy::Unacquire(void) {
  return XDinputDevice::Unacquire();
}

HRESULT WINAPI XDinputDeviceLegacy::GetDeviceState(DWORD cbData, LPVOID lpvData) {
  return XDinputDevice::GetDeviceState(cbData, lpvData);
}

HRESULT WINAPI XDinputDeviceLegacy::GetDeviceData(DWORD a, LPDIDEVICEOBJECTDATA b, LPDWORD c, DWORD d) {
  return XDinputDevice::GetDeviceData(a, b, c, d);
}

HRESULT WINAPI XDinputDeviceLegacy::SetDataFormat(LPCDIDATAFORMAT lpFormat) {
  return XDinputDevice::SetDataFormat(lpFormat);
}

HRESULT WINAPI XDinputDeviceLegacy::SetEventNotification(HANDLE handle) {
  return XDinputDevice::SetEventNotification(handle);
}

HRESULT WINAPI XDinputDeviceLegacy::SetCooperativeLevel(HWND hWnd, DWORD dwFlags) {
  return XDinputDevice::SetCooperativeLevel(hWnd, dwFlags);
}

HRESULT WINAPI XDinputDeviceLegacy::GetObjectInfo(LPDIDEVICEOBJECTINSTANCEW lpdi, DWORD a, DWORD b) {
  return XDinputDevice::GetObjectInfo(lpdi, a, b);
}

HRESULT WINAPI XDinputDeviceLegacy::GetDeviceInfo(LPDIDEVICEINSTANCEW lpdi) {
  return XDinputDevice::GetDeviceInfo(lpdi);
}

HRESULT WINAPI XDinputDeviceLegacy::RunControlPanel(HWND hWnd, DWORD dwFlags) {
  return XDinputDevice::RunControlPanel(hWnd, dwFlags);
}

HRESULT WINAPI XDinputDeviceLegacy::Initialize(HINSTANCE hInstance, DWORD dwFlags, REFGUID riid) {
  return XDinputDevice::Initialize(hInstance, dwFlags, riid);
}