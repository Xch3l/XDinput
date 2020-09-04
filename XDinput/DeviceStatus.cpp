#include "StdAfx.h"
#include "DeviceStatus.h"
#include <CommCtrl.h>
#include <XInput.h>
#include <WinGDI.h>

#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

const WCHAR DevStatClassName[] = L"XDinputDeviceStatus";

///////////////////////////////////////////////////////////////////////

HWND 
  tbLT, tbRT, tbLM, tbRM,
  cbA, cbB, cbX, cbY, cbBack, cbStart, cbLB, cbRB, cbLT, cbRT, cbLS, cbRS,
  cbDUP, cbDLT, cbDDN, cbDRT,
  pText, pLStick, pRStick, pController, pBattery
;
LONG leftMotor[4] = {0};
LONG rightMotor[4] = {0};

int selectedPlayer = 0, lastSelectedPlayer = 4;
BYTE connectedPlayers = 0, lastConnectedPlayers = 255;
XINPUT_STATE status[4];
DWORD lastPacketNumber = -1;

///////////////////////////////////////////////////////////////////////

HRESULT UpdateStatus(void) {
  int index = selectedPlayer & 3;
  WCHAR label[25] = {0};
  XINPUT_STATE s;

  // Reset status flags
  connectedPlayers = 0;

  for(int i = 0; i < 4; i++) {
    ZeroMemory(&s, sizeof(XINPUT_STATE));

    if(XInputGetState(i, &s) == ERROR_SUCCESS)
      connectedPlayers |= (1 << i);

    status[i] = s;
  }

  // Update emblem if a controller has been [dis]connected or a selection changed
  if(lastSelectedPlayer != selectedPlayer || lastConnectedPlayers != connectedPlayers) {
    lastSelectedPlayer = selectedPlayer;
    lastConnectedPlayers = connectedPlayers;
    RedrawWindow(pController, NULL, NULL, RDW_INVALIDATE);

    // update the message too while we're at it
    if(connectedPlayers & (1 << index))
      swprintf_s(label, 25, L"#%d - Connected", index + 1);
    else
      swprintf_s(label, 25, L"#%d - Disconnected", index + 1);
    SetWindowTextW(pText, label);
  }


  // Update status preview
  s = status[index];
  if(lastPacketNumber != s.dwPacketNumber) { // ...if we have an update from XInput
    lastPacketNumber = s.dwPacketNumber;

    SendMessage(tbLT, TBM_SETPOS, TRUE, s.Gamepad.bLeftTrigger);
    SendMessage(tbRT, TBM_SETPOS, TRUE, s.Gamepad.bRightTrigger);
    SendMessage(tbLM, TBM_SETPOS, TRUE, -leftMotor[selectedPlayer & 3]);
    SendMessage(tbRM, TBM_SETPOS, TRUE, rightMotor[selectedPlayer & 3]);

    SendMessage(cbDUP,   BM_SETCHECK, (s.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP ? BST_CHECKED : BST_UNCHECKED), 0);
    SendMessage(cbDLT,   BM_SETCHECK, (s.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT ? BST_CHECKED : BST_UNCHECKED), 0);
    SendMessage(cbDDN,   BM_SETCHECK, (s.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN ? BST_CHECKED : BST_UNCHECKED), 0);
    SendMessage(cbDRT,   BM_SETCHECK, (s.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT ? BST_CHECKED : BST_UNCHECKED), 0);
    SendMessage(cbA,     BM_SETCHECK, (s.Gamepad.wButtons & XINPUT_GAMEPAD_A ? BST_CHECKED : BST_UNCHECKED), 0);
    SendMessage(cbB,     BM_SETCHECK, (s.Gamepad.wButtons & XINPUT_GAMEPAD_B ? BST_CHECKED : BST_UNCHECKED), 0);
    SendMessage(cbX,     BM_SETCHECK, (s.Gamepad.wButtons & XINPUT_GAMEPAD_X ? BST_CHECKED : BST_UNCHECKED), 0);
    SendMessage(cbY,     BM_SETCHECK, (s.Gamepad.wButtons & XINPUT_GAMEPAD_Y ? BST_CHECKED : BST_UNCHECKED), 0);
    SendMessage(cbBack,  BM_SETCHECK, (s.Gamepad.wButtons & XINPUT_GAMEPAD_BACK ? BST_CHECKED : BST_UNCHECKED), 0);
    SendMessage(cbStart, BM_SETCHECK, (s.Gamepad.wButtons & XINPUT_GAMEPAD_START ? BST_CHECKED : BST_UNCHECKED), 0);
    SendMessage(cbLB,    BM_SETCHECK, (s.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER ? BST_CHECKED : BST_UNCHECKED), 0);
    SendMessage(cbRB,    BM_SETCHECK, (s.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER ? BST_CHECKED : BST_UNCHECKED), 0);
    SendMessage(cbLS,    BM_SETCHECK, (s.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB ? BST_CHECKED : BST_UNCHECKED), 0);
    SendMessage(cbRS,    BM_SETCHECK, (s.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB ? BST_CHECKED : BST_UNCHECKED), 0);
    SendMessage(cbLT,    BM_SETCHECK, (s.Gamepad.bLeftTrigger >= 0xC0 ? BST_CHECKED : BST_UNCHECKED), 0);
    SendMessage(cbRT,    BM_SETCHECK, (s.Gamepad.bRightTrigger >= 0xC0 ? BST_CHECKED : BST_UNCHECKED), 0);

    RedrawWindow(pLStick, NULL, NULL, RDW_INVALIDATE);
    RedrawWindow(pRStick, NULL, NULL, RDW_INVALIDATE);
    RedrawWindow(pBattery, NULL, NULL, RDW_INVALIDATE);
  }

  return 0;
}

///////////////////////////////////////////////////////////////////////

DWORD WINAPI ThreadProc(LPVOID lpvParam) {
  HWND hwnd = (HWND) lpvParam;
  DWORD dwMsgResult;
  MSG m;

  ZeroMemory(&m, sizeof(MSG));
  ShowWindow(hwnd, SW_NORMAL);
  SetTimer(hwnd, (int) hwnd, 1000 / 30, nullptr);

  while((dwMsgResult = GetMessage(&m, hwnd, 0, 0)) != 0) {
    if(dwMsgResult == -1) {
      dwMsgResult = GetLastError();
      printf("ThreadProc %08X error 0x%08X\n", (int) hwnd, dwMsgResult);
      continue;
    }

    TranslateMessage(&m);
    DispatchMessage(&m);
  }

  return 0;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
  switch(uMsg) {
    case WM_CREATE:
      return 0;

    case WM_DESTROY:
      KillTimer(hwnd, (int) hwnd);
      PostQuitMessage(0);
      return 0;

    case WM_TIMER:
      if(wParam == (int) hwnd)
        UpdateStatus();
      return 1;

    case WM_DRAWITEM: {
      LPDRAWITEMSTRUCT ds = (LPDRAWITEMSTRUCT) lParam;
      HDC hdc = ds->hDC;
      int x = ds->rcItem.left, y = ds->rcItem.top;
      int ww = ds->rcItem.right - x;
      int hh = ds->rcItem.bottom - y;

      HGDIOBJ oldStroke = SelectObject(hdc, GetStockObject(DC_PEN));
      HGDIOBJ oldFill = SelectObject(hdc, GetStockObject(DC_BRUSH));
      HPEN stroke = NULL;
      HBRUSH fill = NULL;

      if(ds->hwndItem == pController) {
        FillRect(hdc, &ds->rcItem, (HBRUSH) (COLOR_BTNFACE + 1));

        // Draw current controller's arc
        SelectObject(hdc, GetStockObject(BLACK_PEN));

        for(int i = 0; i < 4; i++) {
          if(connectedPlayers & (1 << i))
            SelectObject(hdc, GetStockObject(i == selectedPlayer ? WHITE_BRUSH : LTGRAY_BRUSH));
          else
            SelectObject(hdc, GetStockObject(i == selectedPlayer ? GRAY_BRUSH : DKGRAY_BRUSH));

          if(i == 0) x = 90;
          if(i == 1) x = 0;
          if(i == 2) x = 180;
          if(i == 3) x = 270;

          BeginPath(hdc);
          MoveToEx(hdc, ds->rcItem.left + 25, ds->rcItem.top + 25, NULL);
          AngleArc(hdc, ds->rcItem.left + 25, ds->rcItem.top + 25, 24, (FLOAT) x, 90.0f);
          EndPath(hdc);
          StrokeAndFillPath(hdc);
        }

        // Draw XBOX "emblem"
        SelectObject(hdc, GetStockObject(BLACK_PEN));
        SelectObject(hdc, GetStockObject(GRAY_BRUSH));
        Ellipse(hdc, ds->rcItem.left + 4, ds->rcItem.top + 4, ds->rcItem.right - 4, ds->rcItem.bottom - 4);
      } else
      if(ds->hwndItem == pBattery) {
        RECT r;

        // clear background
        FillRect(hdc, &ds->rcItem, (HBRUSH) (COLOR_BTNFACE + 1));

        // ask the controller for its battery state
        XINPUT_BATTERY_INFORMATION batt;
        ZeroMemory(&batt, sizeof(XINPUT_BATTERY_INFORMATION));
        DWORD br = XInputGetBatteryInformation(selectedPlayer, BATTERY_DEVTYPE_GAMEPAD, &batt);

        if(br == ERROR_SUCCESS && batt.BatteryType > BATTERY_TYPE_WIRED) {
          // battery body
          r.left = ds->rcItem.left;
          r.top  = ds->rcItem.top;
          r.right = ds->rcItem.right - 1;
          r.bottom = ds->rcItem.bottom;
          FillRect(hdc, &r, (HBRUSH) GetStockObject(BLACK_BRUSH));

          // battery tip
          r.left = ds->rcItem.left + 27;
          r.top  = ds->rcItem.top + 6;
          r.right = ds->rcItem.right - 29;
          r.bottom = ds->rcItem.bottom + 14;
          FillRect(hdc, &r, (HBRUSH) GetStockObject(BLACK_BRUSH));

          // battery bars
          r.top = y + 4;
          r.bottom = y + 16;

          if(batt.BatteryLevel >= BATTERY_LEVEL_LOW) {
            r.left = x + 5;
            r.right = x + 10;
            FillRect(hdc, &r, (HBRUSH) GetStockObject(BLACK_BRUSH));
          }

          if(batt.BatteryLevel >= BATTERY_LEVEL_MEDIUM) {
            r.left = x + 12;
            r.right = x + 17;
            FillRect(hdc, &r, (HBRUSH) GetStockObject(BLACK_BRUSH));
          }

          if(batt.BatteryLevel >= BATTERY_LEVEL_FULL) {
            r.left = x + 19;
            r.right = x + 24;
            FillRect(hdc, &r, (HBRUSH) GetStockObject(BLACK_BRUSH));
          }
        }
      }else {
        XINPUT_STATE s = status[selectedPlayer & 3];
        FillRect(hdc, &ds->rcItem, (HBRUSH) (COLOR_WINDOW + 1));

        SelectObject(hdc, GetStockObject(BLACK_PEN));
        SelectObject(hdc, GetStockObject(BLACK_BRUSH));

        // draw border
        MoveToEx(hdc, x, y, NULL);
        LineTo(hdc, ds->rcItem.right - 1, ds->rcItem.top);
        LineTo(hdc, ds->rcItem.right - 1, ds->rcItem.bottom - 1);
        LineTo(hdc, ds->rcItem.left, ds->rcItem.bottom - 1);
        LineTo(hdc, ds->rcItem.left, ds->rcItem.top);

        // get box center
        x = x + (ww / 2);
        y = y + (hh / 2);

        // draw center +
        MoveToEx(hdc, x, y - 5, NULL);
        LineTo(hdc, x, y + 6);
        MoveToEx(hdc, x - 5, y, NULL);
        LineTo(hdc, x + 6, y);

        // get stick position depending on hwnd
        if(ds->hwndItem == pLStick) {
          x = s.Gamepad.sThumbLX;
          y = -s.Gamepad.sThumbLY;
        } else {
          x = s.Gamepad.sThumbRX;
          y = -s.Gamepad.sThumbRY;
        }

        // draw it
        x = (int) (ds->rcItem.left + lerp(50, 100, x / 32768.0F) + 1);
        y = (int) (ds->rcItem.top + lerp(50, 100, y / 32768.0F) + 1);
        Ellipse(hdc, x - 5, y - 5, x + 5, y + 5);
      }

      if(fill)
        DeleteObject(fill);

      if(stroke)
        DeleteObject(stroke);

      SelectObject(hdc, oldStroke);
      SelectObject(hdc, oldFill);
      return TRUE;
    }

    case WM_COMMAND:
      if(HIWORD(wParam) == STN_CLICKED) {
        POINT p;
        GetCursorPos(&p);
        ScreenToClient((HWND) lParam, &p);

        if(p.x < 25 && p.y < 25) selectedPlayer = 0;
        if(p.x > 25 && p.y < 25) selectedPlayer = 1;
        if(p.x < 25 && p.y > 25) selectedPlayer = 2;
        if(p.x > 25 && p.y > 25) selectedPlayer = 3;
      }
      break;
  }

  return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

///////////////////////////////////////////////////////////////////////

HWND CreateTrackbar(HWND hParent, HINSTANCE hInst, UINT xx, UINT yy, UINT ww, UINT hh, UINT iMin, UINT iMax) {
  HWND h = CreateWindowEx(
    0,
    TRACKBAR_CLASS,
    L"TrackbarClass",
    WS_CHILD | WS_VISIBLE | TBS_NOTICKS | TBS_BOTH,
    xx, yy, ww, hh,
    hParent, NULL,
    hInst, NULL
  );
  
  SendMessage(h, TBM_SETRANGEMIN, FALSE, iMin);
  SendMessage(h, TBM_SETRANGEMAX, FALSE, iMax);
  SendMessage(h, TBM_SETPAGESIZE, FALSE, 1);
  SendMessage(h, TBM_SETPOS, TRUE, 0);

  return h;
}

HWND CreateToggleButton(HWND hParent, HINSTANCE hInst, UINT xx, UINT yy, UINT ww, UINT hh, LPCWSTR szText) {
  HWND h = CreateWindowEx(
    0,
    L"BUTTON",
    szText,
    WS_VISIBLE | WS_CHILD | BS_CHECKBOX | BS_PUSHLIKE | BS_TEXT | BS_VCENTER | BS_CENTER,
    xx, yy, ww, hh,
    hParent, NULL,
    hInst, NULL
  );

  return h;
}

///////////////////////////////////////////////////////////////////////

DeviceStatus::DeviceStatus(HINSTANCE hInst) {
  WNDCLASSEX w;
  ZeroMemory(&w, sizeof(WNDCLASSEX));
  ZeroMemory(&leftMotor, sizeof(LONG) * 4);
  ZeroMemory(&rightMotor, sizeof(LONG) * 4);

  w.cbSize = sizeof(WNDCLASSEX);
  w.lpszClassName = DevStatClassName;
  w.lpfnWndProc = WndProc;
  w.hInstance = hInst;
  w.hIcon = LoadIcon(hInst, IDI_APPLICATION);
  w.hIconSm = LoadIcon(hInst, IDI_APPLICATION);
  w.hCursor = LoadCursor(NULL, IDC_ARROW);
  w.hbrBackground = (HBRUSH) (COLOR_BTNFACE + 1);
  RegisterClassEx(&w);

  DWORD ws = WS_POPUP | WS_CAPTION;
  MONITORINFO mi;
  POINT p;
  RECT r;

  GetCursorPos(&p);
  mi.cbSize = sizeof(MONITORINFO);
  HMONITOR hMon = MonitorFromPoint(p, MONITOR_DEFAULTTONEAREST);
  GetMonitorInfo(hMon, &mi);

  r.left = mi.rcWork.left + 10;
  r.top = mi.rcWork.top + 10;
  r.right = 404;
  r.bottom = 325;
  AdjustWindowRect(&r, ws, FALSE);

  hWnd = CreateWindow(DevStatClassName, L"XInput Status", ws, r.left, r.top + 10, r.right, r.bottom, NULL, NULL, hInst, NULL);

  hFont = CreateFont(14, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE,
    DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
    CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("MS Shell Dlg"));

  tbLT = CreateTrackbar(hWnd, hInst, 3, 3, 102, 15, -255, 0);
  tbRT = CreateTrackbar(hWnd, hInst, 291, 3, 102, 15, 0, 255);
  tbLM = CreateTrackbar(hWnd, hInst, 3, 276, 102, 15, 0, 65535);
  tbRM = CreateTrackbar(hWnd, hInst, 291, 276, 102, 15, 0, 65535);
  cbDUP = CreateToggleButton(hWnd, hInst, 111, 168, 30, 30, L"▲");
  cbDLT = CreateToggleButton(hWnd, hInst, 75, 204, 30, 30, L"◀");
  cbDDN = CreateToggleButton(hWnd, hInst, 111, 240, 30, 30, L"▼");
  cbDRT = CreateToggleButton(hWnd, hInst, 147, 204, 30, 30, L"▶");
  cbA = CreateToggleButton(hWnd, hInst, 327, 168, 30, 30, L"A");
  cbB = CreateToggleButton(hWnd, hInst, 363, 132, 30, 30, L"B");
  cbX = CreateToggleButton(hWnd, hInst, 291, 132, 30, 30, L"X");
  cbY = CreateToggleButton(hWnd, hInst, 327, 96, 30, 30, L"Y");
  cbBack = CreateToggleButton(hWnd, hInst, 111, 132, 66, 30, L"BACK");
  cbStart = CreateToggleButton(hWnd, hInst, 219, 132, 66, 30, L"START");
  cbLB = CreateToggleButton(hWnd, hInst, 3, 60, 102, 30, L"LB");
  cbRB = CreateToggleButton(hWnd, hInst, 291, 60, 102, 30, L"RB");
  cbLS = CreateToggleButton(hWnd, hInst, 111, 96, 30, 30, L"LS");
  cbRS = CreateToggleButton(hWnd, hInst, 291, 96, 30, 30, L"RS");
  cbLT = CreateToggleButton(hWnd, hInst, 3, 24, 102, 30, L"LT");
  cbRT = CreateToggleButton(hWnd, hInst, 291, 24, 102, 30, L"RT");

  pText = CreateWindowEx(0, L"STATIC", L"STATUS", WS_CHILD | WS_VISIBLE | SS_CENTER,
    111, 3, 174, 15, hWnd, NULL, hInst, NULL);
  pLStick = CreateWindowEx(0, L"STATIC", L"LStick", WS_CHILD | WS_VISIBLE | SS_OWNERDRAW,
    3, 96, 102, 102, hWnd, NULL, hInst, NULL);
  pRStick = CreateWindowEx(0, L"STATIC", L"RStick", WS_CHILD | WS_VISIBLE | SS_OWNERDRAW,
    220, 168, 102, 102, hWnd, NULL, hInst, NULL);
  pController = CreateWindowEx(0, L"STATIC", L"XBOX", WS_CHILD | WS_VISIBLE | SS_NOTIFY | SS_OWNERDRAW,
    173, 32, 51, 51, hWnd, NULL, hInst, NULL);
  pBattery = CreateWindowEx(0, L"STATIC", L"BATTERY", WS_CHILD | WS_VISIBLE | SS_OWNERDRAW,
    184, 96, 28, 20, hWnd, NULL, hInst, NULL);

  // Apply font to relevant controls
  int controlCount = 21;
  HWND controls[] = {
    cbA, cbB, cbX, cbY, cbBack, cbStart, cbLB, cbRB, cbLT, cbRT, cbLS, cbRS,
    cbDUP, cbDLT, cbDDN, cbDRT, pText, pLStick, pRStick, pController, pBattery
  };

  for(int i = 0; i < controlCount; i++)
    SendMessage(controls[i], WM_SETFONT, (WPARAM) hFont, TRUE);

  if(hWnd)
    hThread = CreateThread(NULL, 0, ThreadProc, hWnd, 0, NULL);
}

DeviceStatus::~DeviceStatus(void) {
  DestroyWindow(tbLM);
  DestroyWindow(tbRM);
  DestroyWindow(tbLT);
  DestroyWindow(tbRT);
  DestroyWindow(cbDUP);
  DestroyWindow(cbDLT);
  DestroyWindow(cbDDN);
  DestroyWindow(cbDRT);
  DestroyWindow(cbA);
  DestroyWindow(cbB);
  DestroyWindow(cbX);
  DestroyWindow(cbY);
  DestroyWindow(cbBack);
  DestroyWindow(cbStart);
  DestroyWindow(cbLB);
  DestroyWindow(cbRB);
  DestroyWindow(cbLT);
  DestroyWindow(cbRT);
  DestroyWindow(cbLS);
  DestroyWindow(cbRS);
  DestroyWindow(pText);
  DestroyWindow(pBattery);
  DestroyWindow(pLStick);
  DestroyWindow(pRStick);
  DestroyWindow(pController);

  DeleteObject(hFont);

  if(hWnd)
    DestroyWindow(hWnd);

  CloseHandle(hThread);
}