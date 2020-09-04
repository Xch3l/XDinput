#include "StdAfx.h"
#include "XDinput7.h"

// Constructor
XDinput7::XDinput7(int isAnsi, LPVOID lpdi) : XDinput(isAnsi, lpdi) {}

// Destructor
XDinput7::~XDinput7() {}

///////////////////////////////////////////////////
/// IUnknown implements
ULONG WINAPI XDinput7::AddRef() {
  return XDinput::AddRef();
}

HRESULT WINAPI XDinput7::QueryInterface(REFIID riid, LPVOID *ppvObject) {
  return XDinput::QueryInterface(riid, ppvObject);
}

ULONG WINAPI XDinput7::Release() {
  return XDinput::Release();
}

///////////////////////////////////////////////////
/// IDirectInput7 implements
HRESULT WINAPI XDinput7::CreateDevice(REFGUID rGuid, LPDIRECTINPUTDEVICE *lplpDirectInputDevice, LPUNKNOWN pUnkOuter) {
  return XDinput::CreateDevice(rGuid, (LPDIRECTINPUTDEVICE8*) lplpDirectInputDevice, pUnkOuter);
}

HRESULT WINAPI XDinput7::EnumDevices(DWORD dwDevType, LPDIENUMDEVICESCALLBACK lpCallback, LPVOID pvRef, DWORD dwFlags) {
  return XDinput::EnumDevices(dwDevType, lpCallback, pvRef, dwFlags);
}

HRESULT WINAPI XDinput7::FindDevice(REFGUID rGuidClass, LPCTSTR ptszName, LPGUID pGuidInstance) {
  return XDinput::FindDevice(rGuidClass, ptszName, pGuidInstance);
}

HRESULT WINAPI XDinput7::GetDeviceStatus(REFGUID rGuidInstance) {
  return XDinput::GetDeviceStatus(rGuidInstance);
}

HRESULT WINAPI XDinput7::Initialize(HINSTANCE hInst, DWORD dwVersion) {
  return XDinput::Initialize(hInst, dwVersion);
}

HRESULT WINAPI XDinput7::RunControlPanel(HWND hwndOwner, DWORD dwFlags) {
  return XDinput::RunControlPanel(hwndOwner, dwFlags);
}

HRESULT WINAPI XDinput7::CreateDeviceEx(REFGUID rGuid, REFIID riid, LPVOID *lplpObject, LPUNKNOWN pUnkOuter) {
  return XDinput::CreateDevice(rGuid, (LPDIRECTINPUTDEVICE8*) lplpObject, pUnkOuter);
}