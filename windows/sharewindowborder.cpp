#include "sharewindowborder.h"
#include <iostream>
#include <Dwmapi.h>
#pragma comment(lib, "dwmapi.lib")

#define SHARING_FRAME_WIDTH (6)
#define ID_TIMER_WINDOW_UPDATE (3)
//#define ID_TIMER_FRAME_COLOR (4)

namespace audio_device_desktop {

  const UINT interval = 20; //ms
  const COLORREF MY_COLOR_KEY = RGB(0, 200, 0);

  //bool tick = false;
  bool minimized = false;

  HWND ShareWindowBorder::_windowHanlde = NULL;
  HWND ShareWindowBorder::_myHwnd = NULL;
  bool ShareWindowBorder::_isRunning = false;
  bool ShareWindowBorder::_windowIsActive = false;

ShareWindowBorder::ShareWindowBorder()
{
}

ShareWindowBorder::~ShareWindowBorder()
{
  stopShowBorder();
  SendMessage(_myHwnd, WM_CLOSE, 0, 0);
}

void ShareWindowBorder::createWnd()
{
  WNDCLASSEX wc = {};
  wc.cbSize = sizeof(WNDCLASSEX);
  wc.style = CS_HREDRAW | CS_VREDRAW;
  wc.lpszClassName = L"MyTransparentFrame";
  wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
  wc.hbrBackground = NULL;

  wc.lpfnWndProc = [](HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) -> LRESULT
  {
    switch (msg)
    {
    case WM_PAINT:
    {
      PAINTSTRUCT ps{};
      HDC hdc = BeginPaint(hwnd, &ps);

      RECT rc{}; GetClientRect(hwnd, &rc);
      //HPEN hPen = CreatePen(PS_SOLID, SHARING_FRAME_WIDTH, tick ? RGB(255, 128, 1) : RGB(255, 201, 14));
      HPEN hPen = CreatePen(PS_SOLID, SHARING_FRAME_WIDTH, RGB(30, 215, 96));
      HBRUSH hBrush = CreateSolidBrush(MY_COLOR_KEY);
      /*HGDIOBJ hOldPen = */SelectObject(hdc, hPen);
      /*HGDIOBJ hOldBrush = */SelectObject(hdc, hBrush);

      Rectangle(hdc, rc.left, rc.top, rc.right, rc.bottom);

      //if (hOldPen)
      //    SelectObject(hdc, hOldPen);
      //if (hOldBrush)
      //    SelectObject(hdc, hOldBrush);
      if (hPen)
        DeleteObject(hPen);
      if (hBrush)
        DeleteObject(hBrush);

      EndPaint(hwnd, &ps);
    }
    break;
    case WM_TIMER:
    {
      if (!IsWindow(_windowHanlde)) {
        stopShowBorder();
      }
      if (ID_TIMER_WINDOW_UPDATE == wp)
      {
        WINDOWPLACEMENT windowPlacement = { sizeof(WINDOWPLACEMENT), };
        if (::GetWindowPlacement(ShareWindowBorder::_windowHanlde,
          &windowPlacement))
        {
          if (windowPlacement.showCmd == SW_SHOWMINIMIZED
            || windowPlacement.showCmd == SW_SHOWMAXIMIZED
            || !IsWindowVisible(ShareWindowBorder::_windowHanlde))
          {
            ShowWindow(hwnd, SW_HIDE);
            minimized = true;
          }
          else
          {
            RECT rect = {};
            DwmGetWindowAttribute(_windowHanlde,
              DWMWA_EXTENDED_FRAME_BOUNDS, &rect,
              sizeof(rect));
            bool maximized = false;
            MONITORINFO monInfo;
            monInfo.cbSize = sizeof(MONITORINFO);
            GetMonitorInfoW(MonitorFromWindow(_windowHanlde, MONITOR_DEFAULTTONEAREST), &monInfo);
            if (_windowHanlde != NULL && ::IsZoomed(_windowHanlde))
            {
              rect.left = monInfo.rcWork.left;
              rect.top = monInfo.rcWork.top;
              rect.bottom = monInfo.rcWork.bottom > rect.bottom ? rect.bottom : monInfo.rcWork.bottom;
              rect.right = monInfo.rcWork.right > rect.right ? rect.right : monInfo.rcWork.right;
              maximized = true;
            }
            //go after window
            if (minimized)
            {
              ShowWindow(hwnd, SW_HIDE);
              minimized = false;
            }
            else {
              int offset = maximized ? 0 : SHARING_FRAME_WIDTH - 3;
              SetWindowPos(hwnd, 0, rect.left - offset, rect.top - offset,
                rect.right - rect.left + 2 * offset,
                rect.bottom - rect.top + 2 * offset, SWP_NOZORDER);
              SetWindowPos(hwnd, GetNextWindow(_windowHanlde, GW_HWNDPREV), 0, 0, 0, 0,
                SWP_SHOWWINDOW | SWP_NOMOVE | SWP_NOSIZE);
            }
          }
        }
        else {
          const DWORD code = GetLastError();
          if (code != NO_ERROR) {
            std::cout << "window error: " << code << std::endl;
            if (code == ERROR_INVALID_WINDOW_HANDLE) {
              stopShowBorder();
            }
          }
        }
      }
      //else if (ID_TIMER_FRAME_COLOR == wp)
      //{
      //  //tick = !tick;
      //  ::RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE);
      //}
      break;
    }
    case WM_START_WINDOW_BORDER:
    {
      HWND window_id = (HWND)wp;
      startShowBorder(window_id);
    }
    break;
    case WM_STOP_WINDOW_BORDER:
      stopShowBorder();
      break;
    case WM_DESTROY:
      PostQuitMessage(0);
      break;
    default:
      return DefWindowProcW(hwnd, msg, wp, lp);
    }

    return 0;
  };

  RegisterClassEx(&wc);

  _myHwnd = CreateWindowExW(WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_LAYERED | WS_EX_TRANSPARENT, wc.lpszClassName, L"", WS_POPUP | WS_VISIBLE | WS_DISABLED,
    0, 0, 0, 0, nullptr, nullptr, nullptr, nullptr);

  LONG style = GetWindowLong(_myHwnd, GWL_EXSTYLE);
  SetWindowLong(_myHwnd, GWL_EXSTYLE,
    style | WS_EX_TRANSPARENT | WS_EX_LAYERED);

  ::MSG msg;
  while (::GetMessage(&msg, nullptr, 0, 0)) {
    ::TranslateMessage(&msg);
    ::DispatchMessage(&msg);
  }
}

void ShareWindowBorder::create() {
  std::thread th(&ShareWindowBorder::createWnd, this);
  th.detach();
}

void ShareWindowBorder::stopShowBorder() {
  _isRunning = false;
  ::KillTimer(_myHwnd, ID_TIMER_WINDOW_UPDATE);
  //::KillTimer(hwnd, ID_TIMER_FRAME_COLOR);
  ShowWindow(_myHwnd, SW_HIDE);

  std::cout << "--------stopShowBorder--------" << std::endl;
}

void ShareWindowBorder::startShowBorder(HWND windowId) {
  if (_isRunning) {
    stopShowBorder();
  }
  _windowHanlde = windowId;
  if (_windowHanlde && IsWindow(windowId)) {
    ::SetTimer(_myHwnd, ID_TIMER_WINDOW_UPDATE, interval, NULL);
    //::SetTimer(_myHwnd, ID_TIMER_FRAME_COLOR, 500, NULL);
    SetLayeredWindowAttributes(_myHwnd, MY_COLOR_KEY, 255, LWA_COLORKEY);
    ShowWindow(_myHwnd, SW_SHOW);
    _isRunning = true;
  }
  else {
    stopShowBorder();
    std::cout << "windowId is null." << std::endl;
  }
}

}