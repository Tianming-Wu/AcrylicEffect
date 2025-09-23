#pragma once
#include <windows.h>

#ifdef _MSC_VER
#include <dwmapi.h>
#else
constexpr auto DWMWA_SYSTEMBACKDROP_TYPE = 38;
typedef enum DWM_SYSTEMBACKDROP_TYPE {
  DWMSBT_AUTO, // 0 由系统决定背景样式，默认不设置即为该值
  DWMSBT_NONE, // 1 无背景样式，通常为白色背景或是对应主题色
  DWMSBT_MAINWINDOW, // 2 云母(mica)：云母样式
  DWMSBT_TRANSIENTWINDOW, // 3 亚克力(Acrylic)：亚克力
  DWMSBT_TABBEDWINDOW // 4 特殊云母（MicaAlt）：特殊云母样式
};
#endif

void __declspec(dllimport) SetAcrylicEffect(HWND hwnd, DWM_SYSTEMBACKDROP_TYPE type);