#include "acrylicEffect_private.h"
#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")

// Acrylic 需要 Windows 10 1803 (RS4) 及以上并启用混合
// 参考：https://docs.microsoft.com/en-us/windows/win32/api/dwmapi/ne-dwmapi-dwm_systembackdrop_type
// 这里只做简单实现，生产环境建议做版本检测

// DWM_BLURBEHIND 用于启用 Blur 效果
// DWM_SYSTEMBACKDROP_TYPE 用于 Acrylic（需要 Windows 11 以上）

// 启用模糊（Blur）效果
void EnableBlurEffect(HWND hwnd) {
    DWM_BLURBEHIND bb = {};
    bb.dwFlags = DWM_BB_ENABLE;
    bb.fEnable = TRUE;
    bb.hRgnBlur = nullptr;
    DwmEnableBlurBehindWindow(hwnd, &bb);
}

// 禁用模糊（Blur）效果
void DisableBlurEffect(HWND hwnd) {
    DWM_BLURBEHIND bb = {};
    bb.dwFlags = DWM_BB_ENABLE;
    bb.fEnable = FALSE;
    bb.hRgnBlur = nullptr;
    DwmEnableBlurBehindWindow(hwnd, &bb);
}

// 启用 Acrylic 效果（仅 Windows 11 及部分 Windows 10 版本支持）
// 通过 DwmSetWindowAttribute 设置 DWMWA_SYSTEMBACKDROP_TYPE
void EnableAcrylicEffect(HWND hwnd) {
    // Windows 11 22000+ 支持 DWMWA_SYSTEMBACKDROP_TYPE
    // 3 = DWM_SYSTEMBACKDROP_TYPE_ACRYLIC
    const DWORD DWMWA_SYSTEMBACKDROP_TYPE = 38; // Windows 11 21H2 及以上
    const int DWM_SYSTEMBACKDROP_TYPE_ACRYLIC = 3;
    DwmSetWindowAttribute(hwnd, DWMWA_SYSTEMBACKDROP_TYPE, &DWM_SYSTEMBACKDROP_TYPE_ACRYLIC, sizeof(DWM_SYSTEMBACKDROP_TYPE_ACRYLIC));
}

// 禁用 Acrylic 效果（恢复为无 backdrop）
void DisableAcrylicEffect(HWND hwnd) {
    const DWORD DWMWA_SYSTEMBACKDROP_TYPE = 38;
    const int DWM_SYSTEMBACKDROP_TYPE_NONE = 0;
    DwmSetWindowAttribute(hwnd, DWMWA_SYSTEMBACKDROP_TYPE, &DWM_SYSTEMBACKDROP_TYPE_NONE, sizeof(DWM_SYSTEMBACKDROP_TYPE_NONE));
}