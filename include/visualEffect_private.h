#pragma once
#include <windows.h>
#include <functional>

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

// Accent/Acrylic (SetWindowCompositionAttribute) definitions
enum ACCENT_STATE {
    ACCENT_DISABLED = 0,
    ACCENT_ENABLE_GRADIENT = 1,
    ACCENT_ENABLE_TRANSPARENTGRADIENT = 2,
    ACCENT_ENABLE_BLURBEHIND = 3,
    ACCENT_ENABLE_ACRYLICBLURBEHIND = 4,
    ACCENT_ENABLE_HOSTBACKDROP = 5,
    ACCENT_INVALID_STATE = 6
};

struct ACCENT_POLICY {
    ACCENT_STATE AccentState;
    DWORD AccentFlags;
    DWORD GradientColor; // ARGB
    DWORD AnimationId;
};

struct WINDOWCOMPOSITIONATTRIBDATA {
    DWORD Attrib;
    PVOID pvData;
    SIZE_T cbData;
};

using pSetWindowCompositionAttribute = BOOL (WINAPI *)(HWND, WINDOWCOMPOSITIONATTRIBDATA*);
constexpr DWORD WCA_ACCENT_POLICY = 19;

// Internal: Power mode tracking initialization/cleanup
void __declspec(dllexport) InitPowerModeMonitoring();
void __declspec(dllexport) CleanupPowerModeMonitoring();

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


bool __declspec(dllexport) SetAcrylicEffect(HWND hwnd, EffectType type, CornerPreference corner);

bool __declspec(dllexport) SetBlurEffect(HWND hwnd, Region region);

// Power mode callback
using BlurModeCallback = std::function<void(bool)>;
void __declspec(dllexport) SetBlurModeChangeCallback(BlurModeCallback callback);