#pragma once
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

#define DWMWA_WINDOW_CORNER_PREFERENCE 33
enum DWM_WINDOW_CORNER_PREFERENCE {
    DWMWCP_DEFAULT = 0,
    DWMWCP_DONOTROUND = 1,
    DWMWCP_ROUND = 2,
    DWMWCP_ROUNDSMALL = 3
};

typedef HRESULT (WINAPI *pExtendFrameIntoClientArea)(HWND, const MARGINS*);
typedef HRESULT (WINAPI *pSetWindowAttribute)(HWND, DWORD, LPCVOID, DWORD);
const extern pExtendFrameIntoClientArea DwmExtendFrameIntoClientArea;
const extern pSetWindowAttribute        DwmSetWindowAttribute;

#endif

class __declspec(dllexport) Region {
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


void __declspec(dllexport) SetAcrylicEffect(HWND hwnd, DWM_SYSTEMBACKDROP_TYPE type);
bool __declspec(dllexport) SetBlurEffect(HWND hwnd, Region region);
HRESULT __declspec(dllexport) SetBlurEffectHr(HWND hwnd, Region region);
bool __declspec(dllexport) SetMicaEffect(HWND hwnd, DWM_SYSTEMBACKDROP_TYPE type, DWM_WINDOW_CORNER_PREFERENCE corner);