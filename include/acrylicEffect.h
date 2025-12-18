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

#define DWMWA_WINDOW_CORNER_PREFERENCE 33
enum DWM_WINDOW_CORNER_PREFERENCE {
    DWMWCP_DEFAULT = 0,
    DWMWCP_DONOTROUND = 1,
    DWMWCP_ROUND = 2,
    DWMWCP_ROUNDSMALL = 3
};

#endif


///TODO: This class is not moved safely yet, make sure to do that later.
class __declspec(dllimport) Region {
    HRGN hRgn = nullptr;
public:
    Region();
    ~Region();
    
    // copy / move constructors & operators
    Region(const Region&) = delete;
    Region& operator=(const Region&) = delete;

    Region(Region&& other) noexcept;
    Region& operator=(Region&& other) noexcept;
    
    [[nodiscard]] static Region createRounded(int left, int top, int right, int bottom, int radius);
    inline HRGN handle() const { return hRgn; }
};

void __declspec(dllimport) SetAcrylicEffect(HWND hwnd, DWM_SYSTEMBACKDROP_TYPE type);
bool __declspec(dllimport) SetBlurEffect(HWND hwnd, Region region);
HRESULT __declspec(dllimport) SetBlurEffectHr(HWND hwnd, Region region);
bool __declspec(dllimport) SetMicaEffect(HWND hwnd, DWM_SYSTEMBACKDROP_TYPE type = DWMSBT_MAINWINDOW, DWM_WINDOW_CORNER_PREFERENCE corner = DWMWCP_ROUND);