#pragma once

class DeviceStatus {
  public:
    DeviceStatus(HINSTANCE);
    ~DeviceStatus(void);
    HWND hWnd;
    HANDLE hThread;
    HFONT hFont;

  private:
};