#include <windows.h>
#ifdef _MSC_VER
#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")
#else
constexpr auto DWMWA_SYSTEMBACKDROP_TYPE = 38;
typedef enum DWM_SYSTEMBACKDROP_TYPE {
  DWMSBT_AUTO,
  DWMSBT_NONE,
  DWMSBT_MAINWINDOW,
  DWMSBT_TRANSIENTWINDOW,
  DWMSBT_TABBEDWINDOW
};
typedef HRESULT (WINAPI *pExtendFrameIntoClientArea)(HWND, const MARGINS*);
typedef HRESULT (WINAPI *pSetWindowAttribute)(HWND, DWORD, LPCVOID, DWORD);
const extern pExtendFrameIntoClientArea DwmExtendFrameIntoClientArea;
const extern pSetWindowAttribute        DwmSetWindowAttribute;
#endif