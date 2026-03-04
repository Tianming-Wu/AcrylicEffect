#pragma once
#include <windows.h>
#include <functional>

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

    Region(HRGN hRgn);
    
    [[nodiscard]] static Region createRounded(int left, int top, int right, int bottom, int radius);
    [[nodiscard]] static Region createRoundedScaled(int left, int top, int right, int bottom, int radius, double scalingFactor);
    inline HRGN handle() const { return hRgn; }
};

// More friendly names
enum EffectType : int {
    EffectType_Default = DWMSBT_AUTO,
    EffectType_None = DWMSBT_NONE,
    EffectType_Mica = DWMSBT_MAINWINDOW,
    EffectType_Acrylic = DWMSBT_TRANSIENTWINDOW,
    EffectType_MicaAlt = DWMSBT_TABBEDWINDOW
};

enum CornerPreference : int {
    Corner_Default = DWMWCP_DEFAULT,
    Corner_NoRound = DWMWCP_DONOTROUND,
    Corner_Round = DWMWCP_ROUND,
    Corner_RoundSmall = DWMWCP_ROUNDSMALL
};


// void __declspec(dllimport) SetAcrylicEffect(HWND hwnd, DWM_SYSTEMBACKDROP_TYPE type); // Older version, but the newer one is already compatible
bool __declspec(dllimport) SetAcrylicEffect(HWND hwnd, EffectType type = EffectType_Mica, CornerPreference corner = Corner_Round);

// Older visual effect versions
bool __declspec(dllimport) SetBlurEffect(HWND hwnd, Region region);

// Power mode / effect mode management
// Callback: bool param indicates current power saving mode (true = saving, false = normal)
using BlurModeCallback = std::function<void(bool)>;
void __declspec(dllimport) SetBlurModeChangeCallback(BlurModeCallback callback);