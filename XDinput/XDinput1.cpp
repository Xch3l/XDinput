#include "StdAfx.h"
#include "XDinput1.h"

// Constructor
XDinput1::XDinput1(int isAnsi, LPVOID lpdi) {
  xdiBase = new XDinput(isAnsi, lpdi);
}

// Destructor
XDinput1::~XDinput1() {
  delete xdiBase;
}

///////////////////////////////////////////////////
/// IUnknown implements
HRESULT WINAPI XDinput1::QueryInterface(REFIID riid, LPVOID *ppvObject) {
  return xdiBase->QueryInterface(riid, ppvObject);
}

ULONG WINAPI XDinput1::AddRef() {
  return xdiBase->AddRef();
}

ULONG WINAPI XDinput1::Release() {
  return xdiBase->Release();
}

///////////////////////////////////////////////////
/// IDirectInput implements
HRESULT WINAPI XDinput1::CreateDevice(REFGUID rGuid, LPDIRECTINPUTDEVICE *lplpDirectInputDevice, LPUNKNOWN pUnkOuter) {
  return xdiBase->CreateDevice(rGuid, (LPDIRECTINPUTDEVICE8*) lplpDirectInputDevice, pUnkOuter);
}

HRESULT WINAPI XDinput1::EnumDevices(DWORD dwDevType, LPDIENUMDEVICESCALLBACK lpCallback, LPVOID pvRef, DWORD dwFlags) {
  return xdiBase->EnumDevices(dwDevType, lpCallback, pvRef, dwFlags);
}

HRESULT WINAPI XDinput1::GetDeviceStatus(REFGUID rGuidInstance) {
  return xdiBase->GetDeviceStatus(rGuidInstance);
}

HRESULT WINAPI XDinput1::RunControlPanel(HWND hwndOwner, DWORD dwFlags) {
  return xdiBase->RunControlPanel(hwndOwner, dwFlags);
}

HRESULT WINAPI XDinput1::Initialize(HINSTANCE hInst, DWORD dwVersion) {
  return xdiBase->Initialize(hInst, dwVersion);
}
