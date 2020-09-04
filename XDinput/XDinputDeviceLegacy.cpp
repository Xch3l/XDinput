#include "StdAfx.h"
#include "XDinputDeviceLegacy.h"

XDinputDeviceLegacy::XDinputDeviceLegacy(int index, BOOL isAnsi) {
  xdiBase = new XDinputDevice(index, isAnsi);
}

XDinputDeviceLegacy::~XDinputDeviceLegacy(void) {
  //delete xdiBase;
}

///////////////////////////////////////////////////
/// IUnknown implements
ULONG WINAPI XDinputDeviceLegacy::AddRef() {
  return xdiBase->AddRef();
}

HRESULT WINAPI XDinputDeviceLegacy::QueryInterface(REFIID riid, LPVOID *ppvObject) {
  return xdiBase->QueryInterface(riid, ppvObject);
}

ULONG WINAPI XDinputDeviceLegacy::Release() {
  return xdiBase->Release();
}

///////////////////////////////////////////////////
/// IDirectInputDevice implements
HRESULT WINAPI XDinputDeviceLegacy::GetCapabilities(LPDIDEVCAPS lpDIDevCaps) {
  return xdiBase->GetCapabilities(lpDIDevCaps);
}

HRESULT WINAPI XDinputDeviceLegacy::EnumObjects(LPDIENUMDEVICEOBJECTSCALLBACKW lpCallback, LPVOID pvRef, DWORD dwFlags) {
  return xdiBase->EnumObjects(lpCallback, pvRef, dwFlags);
}

HRESULT WINAPI XDinputDeviceLegacy::GetProperty(REFGUID riid, LPDIPROPHEADER lpdiPropHeader) {
  return xdiBase->GetProperty(riid, lpdiPropHeader);
}

HRESULT WINAPI XDinputDeviceLegacy::SetProperty(REFGUID rGuidProp, LPCDIPROPHEADER pdiph) {
  return xdiBase->SetProperty(rGuidProp, pdiph);
}

HRESULT WINAPI XDinputDeviceLegacy::Acquire(void) {
  return xdiBase->Acquire();
}

HRESULT WINAPI XDinputDeviceLegacy::Unacquire(void) {
  return xdiBase->Unacquire();
}

HRESULT WINAPI XDinputDeviceLegacy::GetDeviceState(DWORD cbData, LPVOID lpvData) {
  return xdiBase->GetDeviceState(cbData, lpvData);
}

HRESULT WINAPI XDinputDeviceLegacy::GetDeviceData(DWORD a, LPDIDEVICEOBJECTDATA b, LPDWORD c, DWORD d) {
  return xdiBase->GetDeviceData(a, b, c, d);
}

HRESULT WINAPI XDinputDeviceLegacy::SetDataFormat(LPCDIDATAFORMAT lpFormat) {
  return xdiBase->SetDataFormat(lpFormat);
}

HRESULT WINAPI XDinputDeviceLegacy::SetEventNotification(HANDLE handle) {
  return xdiBase->SetEventNotification(handle);
}

HRESULT WINAPI XDinputDeviceLegacy::SetCooperativeLevel(HWND hWnd, DWORD dwFlags) {
  return xdiBase->SetCooperativeLevel(hWnd, dwFlags);
}

HRESULT WINAPI XDinputDeviceLegacy::GetObjectInfo(LPDIDEVICEOBJECTINSTANCEW lpdi, DWORD a, DWORD b) {
  return xdiBase->GetObjectInfo(lpdi, a, b);
}

HRESULT WINAPI XDinputDeviceLegacy::GetDeviceInfo(LPDIDEVICEINSTANCEW lpdi) {
  return xdiBase->GetDeviceInfo(lpdi);
}

HRESULT WINAPI XDinputDeviceLegacy::RunControlPanel(HWND hWnd, DWORD dwFlags) {
  return xdiBase->RunControlPanel(hWnd, dwFlags);
}

HRESULT WINAPI XDinputDeviceLegacy::Initialize(HINSTANCE hInstance, DWORD dwFlags, REFGUID riid) {
  return xdiBase->Initialize(hInstance, dwFlags, riid);
}
