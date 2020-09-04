#include "StdAfx.h"
#include "XDinput2.h"

// Constructor
XDinput2::XDinput2(int isAnsi, LPVOID lpdi) {
  xdiBase = new XDinput(isAnsi, lpdi);
}

// Destructor
XDinput2::~XDinput2() {
  delete xdiBase;
}

///////////////////////////////////////////////////
/// IUnknown implements
HRESULT WINAPI XDinput2::QueryInterface(REFIID riid, LPVOID *ppvObject) {
  return xdiBase->QueryInterface(riid, ppvObject);
}

ULONG WINAPI XDinput2::AddRef() {
  return xdiBase->AddRef();
}

ULONG WINAPI XDinput2::Release() {
  return xdiBase->Release();
}

///////////////////////////////////////////////////
/// IDirectInput2 implements
HRESULT WINAPI XDinput2::CreateDevice(REFGUID rGuid, LPDIRECTINPUTDEVICE *lplpDirectInputDevice, LPUNKNOWN pUnkOuter) {
  return xdiBase->CreateDevice(rGuid, (LPDIRECTINPUTDEVICE8*) lplpDirectInputDevice, pUnkOuter);
}

HRESULT WINAPI XDinput2::EnumDevices(DWORD dwDevType, LPDIENUMDEVICESCALLBACK lpCallback, LPVOID pvRef, DWORD dwFlags) {
  return xdiBase->EnumDevices(dwDevType, lpCallback, pvRef, dwFlags);
}

HRESULT WINAPI XDinput2::GetDeviceStatus(REFGUID rGuidInstance) {
  return xdiBase->GetDeviceStatus(rGuidInstance);
}

HRESULT WINAPI XDinput2::RunControlPanel(HWND hwndOwner, DWORD dwFlags) {
  return xdiBase->RunControlPanel(hwndOwner, dwFlags);
}
HRESULT WINAPI XDinput2::Initialize(HINSTANCE hInst, DWORD dwVersion) {
  return xdiBase->Initialize(hInst, dwVersion);
}

HRESULT WINAPI XDinput2::FindDevice(REFGUID rGuidClass, LPCTSTR ptszName, LPGUID pGuidInstance) {
  return xdiBase->FindDevice(rGuidClass, ptszName, pGuidInstance);
}