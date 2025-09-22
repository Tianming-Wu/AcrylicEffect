#pragma once
#include <windows.h>

// 启用 Acrylic 效果
__declspec(dllimport) void EnableAcrylicEffect(HWND hwnd);

// 禁用 Acrylic 效果（恢复常规窗口）
__declspec(dllimport) void DisableAcrylicEffect(HWND hwnd);

// 启用 Blur（模糊）效果
__declspec(dllimport) void EnableBlurEffect(HWND hwnd);

// 禁用 Blur 效果（恢复常规窗口）
__declspec(dllimport) void DisableBlurEffect(HWND hwnd);