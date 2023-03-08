#ifndef __SHARE_WINDOW_BORDER_H__
#define __SHARE_WINDOW_BORDER_H__

#include <Windows.h>
#include <thread>
#define WM_START_WINDOW_BORDER (WM_USER + 4)
#define WM_STOP_WINDOW_BORDER (WM_USER + 5)

namespace audio_device_desktop {

  class ShareWindowBorder
  {
  public:
    ShareWindowBorder();
    ~ShareWindowBorder();

    void createWnd();
    void create();
    HWND getHwnd() {
      return _myHwnd;
    }
    bool isRunning() {
      return _isRunning;
    }

  private:
    static void startShowBorder(HWND windowId);
    static void stopShowBorder();

  private:
    static bool _isRunning;
    static HWND _myHwnd;
    static HWND _windowHanlde;
    static bool _windowIsActive;
  };
}

#endif // !__SHARE_WINDOW_BORDER_H__
