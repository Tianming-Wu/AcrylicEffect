#include "visualEffect_private.h"
#include <stdexcept>
#include <comdef.h>
#include <mutex>
#include <winuser.h>
// #include "visualEffect.h"

HMODULE dwmapi = nullptr;

// Global callback and sync
static BlurModeCallback g_blurModeCallback = nullptr;
static std::mutex g_callbackMutex;
static HWND g_powerModeWindow = nullptr;
static constexpr WCHAR POWER_MODE_WINDOW_CLASS[] = L"AcrylicEffect_PowerMonitor";
static bool g_isInPowerSaveMode = false;

// Hidden window for power mode notifications
LRESULT CALLBACK PowerModeWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg) {
        case WM_POWERBROADCAST: {
            if (wParam == PBT_POWERSETTINGCHANGE) {
                const POWERBROADCAST_SETTING* pbs = reinterpret_cast<POWERBROADCAST_SETTING*>(lParam);
                if (pbs && pbs->PowerSetting == GUID_POWER_SAVING_STATUS) {
                    // Check current power status instead of assuming the value
                    SYSTEM_POWER_STATUS sps = {};
                    if (GetSystemPowerStatus(&sps)) {
                        bool isSaving = (sps.SystemStatusFlag & 1) != 0;
                        if (isSaving != g_isInPowerSaveMode) {
                            g_isInPowerSaveMode = isSaving;
                            {
                                std::lock_guard<std::mutex> lock(g_callbackMutex);
                                if (g_blurModeCallback) {
                                    g_blurModeCallback(isSaving);
                                }
                            }
                        }
                    }
                }
            }
            return TRUE;
        }
        case WM_SETTINGCHANGE: {
            // Fallback for Windows versions that don't support WM_POWERBROADCAST
            if (wParam == 0 && lParam) {
                const wchar_t* setting = reinterpret_cast<const wchar_t*>(lParam);
                if (wcscmp(setting, L"PowerSavingStatus") == 0) {
                    std::lock_guard<std::mutex> lock(g_callbackMutex);
                    if (g_blurModeCallback) {
                        // Query current power mode
                        SYSTEM_POWER_STATUS sps = {};
                        if (GetSystemPowerStatus(&sps)) {
                            bool isSaving = (sps.SystemStatusFlag & 1) != 0;
                            if (isSaving != g_isInPowerSaveMode) {
                                g_isInPowerSaveMode = isSaving;
                                g_blurModeCallback(isSaving);
                            }
                        }
                    }
                }
            }
            return TRUE;
        }
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
}

namespace {
    // Fallback for layered windows (per-pixel alpha) using Accent/Acrylic path
    HRESULT ApplyAccentBlur(HWND hwnd, bool useAcrylic)
    {
        static HMODULE hUser = LoadLibraryW(L"user32.dll");
        if (!hUser) {
            return HRESULT_FROM_WIN32(GetLastError());
        }

        static const auto fnSetWca = reinterpret_cast<pSetWindowCompositionAttribute>(
            GetProcAddress(hUser, "SetWindowCompositionAttribute"));
        if (!fnSetWca) {
            return HRESULT_FROM_WIN32(GetLastError());
        }

        ACCENT_POLICY policy{};
        policy.AccentState = useAcrylic ? ACCENT_ENABLE_ACRYLICBLURBEHIND : ACCENT_ENABLE_BLURBEHIND;
        policy.AccentFlags = 0;
        // Default acrylic color: semi-transparent white (0xCC alpha)
        policy.GradientColor = (DWORD(0xCC) << 24) | 0x00FFFFFF;
        policy.AnimationId = 0;

        WINDOWCOMPOSITIONATTRIBDATA data{};
        data.Attrib = WCA_ACCENT_POLICY;
        data.pvData = &policy;
        data.cbData = sizeof(policy);

        const BOOL ok = fnSetWca(hwnd, &data);
        if (!ok) {
            return HRESULT_FROM_WIN32(GetLastError());
        }
        return S_OK;
    }
}

// DLL api entry point
BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD fdwReason, LPVOID lpReserved)
{
    switch(fdwReason) 
    { 
        case DLL_PROCESS_ATTACH:
            InitPowerModeMonitoring();
            #ifndef _MSC_VER
            dwmapi = LoadLibrary(TEXT("dwmapi.dll"));
            const auto pfn_ExtendFrameIntoClientArea = GetProcAddress(dwmapi, "DwmExtendFrameIntoClientArea");
            const auto pfn_SetWindowAttribute = GetProcAddress(dwmapi, "DwmSetWindowAttribute");
            const auto DwmExtendFrameIntoClientArea = reinterpret_cast<pExtendFrameIntoClientArea>(pfn_ExtendFrameIntoClientArea);
            const auto DwmSetWindowAttribute = reinterpret_cast<pSetWindowAttribute>(pfn_SetWindowAttribute);
            #endif
            break;

        case DLL_THREAD_ATTACH:
         // Do thread-specific initialization.
            break;

        case DLL_THREAD_DETACH:
         // Do thread-specific cleanup.
            break;

        case DLL_PROCESS_DETACH:
            CleanupPowerModeMonitoring();
            if (lpReserved != nullptr) break; // do not do cleanup if process termination scenario
            #ifndef _MSC_VER
            FreeLibrary(dwmapi);
            #endif
        
            break;
    }
    return TRUE;  // Successful DLL_PROCESS_ATTACH.
}

// Older version
// void __declspec(dllexport) SetAcrylicEffect(HWND hwnd, DWM_SYSTEMBACKDROP_TYPE type)
// {
//     constexpr auto margins = MARGINS{-1, -1, -1, -1}; //若值其中某一值为负数，则会扩展到整个窗口，正好满足需求
//     DwmSetWindowAttribute(hwnd, DWMWA_SYSTEMBACKDROP_TYPE, &type, sizeof(type));
// }

bool __declspec(dllexport) SetAcrylicEffect(HWND hwnd, EffectType type, CornerPreference corner)
{
    if(!hwnd) return false;

    HRESULT hr1, hr2;

    DWM_SYSTEMBACKDROP_TYPE backdrop = static_cast<DWM_SYSTEMBACKDROP_TYPE>(type);
    hr1 = DwmSetWindowAttribute(hwnd, DWMWA_SYSTEMBACKDROP_TYPE, &backdrop, sizeof(backdrop));

    // 设置圆角偏好
    if(corner != Corner_Default) {
        // 仅当不是默认值时才设置
        DWM_WINDOW_CORNER_PREFERENCE cp = static_cast<DWM_WINDOW_CORNER_PREFERENCE>(corner);
        hr2 = DwmSetWindowAttribute(hwnd, DWMWA_WINDOW_CORNER_PREFERENCE, &cp, sizeof(cp));
    }
    else {
        hr2 = S_OK; // 默认值不设置，视为成功
    }

    return SUCCEEDED(hr1) && SUCCEEDED(hr2);
}

bool __declspec(dllexport) SetBlurEffect(HWND hwnd, Region region)
{
    if(hwnd == nullptr) throw std::invalid_argument("hwnd is null");

    const LONG_PTR exStyle = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
    if (exStyle & WS_EX_LAYERED) {
        const HRESULT hr = ApplyAccentBlur(hwnd, /*useAcrylic=*/true);
        if (FAILED(hr)) {
            _bstr_t error_msg(_com_error(hr).ErrorMessage());
            throw std::runtime_error(static_cast<const char*>(error_msg));
        }
        return true;
    }

    DWM_BLURBEHIND bb = {};
    bb.dwFlags = DWM_BB_ENABLE | DWM_BB_BLURREGION;
    bb.fEnable = TRUE;
    bb.hRgnBlur = region.handle();

    HRESULT hr = DwmEnableBlurBehindWindow(hwnd, &bb);

    if(FAILED(hr)) {
        _bstr_t error_msg(_com_error(hr).ErrorMessage());
        throw std::runtime_error(static_cast<const char*>(error_msg));
    }

    return SUCCEEDED(hr);
}

Region::Region()
{
}

Region::~Region()
{
    if(hRgn != nullptr) {
        DeleteObject(hRgn);
    }
}

Region Region::createRounded(int left, int top, int right, int bottom, int radius)
{
    Region region;
    if(region.hRgn) {
        DeleteObject(region.hRgn);
        region.hRgn = nullptr;
    }

    region.hRgn = CreateRoundRectRgn(left, top, right, bottom, radius, radius);
    return region;
}

Region Region::createRoundedScaled(int left, int top, int right, int bottom, int radius, double scalingFactor)
{
    Region region;
    if(region.hRgn) {
        DeleteObject(region.hRgn);
        region.hRgn = nullptr;
    }

    #define _cScale(x) static_cast<int>((x) * (scalingFactor))

    region.hRgn = CreateRoundRectRgn(_cScale(left), _cScale(top), _cScale(right), _cScale(bottom), _cScale(radius), _cScale(radius));
    return region;
}

Region::Region(Region&& other) noexcept {
    hRgn = other.hRgn;
    other.hRgn = nullptr; // set the original one to null to avoid double deletion
}

Region& Region::operator=(Region&& other) noexcept {
    if (this != &other) {
        if (hRgn != nullptr) {
            DeleteObject(hRgn);
        }
        hRgn = other.hRgn;
        other.hRgn = nullptr;
    }
    return *this;
}

Region::Region(HRGN hRgn)
{
    this->hRgn = hRgn;
}

// Power mode monitoring implementation
void __declspec(dllexport) InitPowerModeMonitoring()
{
    if (g_powerModeWindow != nullptr) {
        return; // Already initialized
    }

    // Register window class
    WNDCLASSW wc = {};
    wc.lpfnWndProc = PowerModeWindowProc;
    wc.lpszClassName = POWER_MODE_WINDOW_CLASS;
    RegisterClassW(&wc);

    // Create hidden message-only window
    g_powerModeWindow = CreateWindowW(
        POWER_MODE_WINDOW_CLASS,
        L"",
        0,
        0, 0, 0, 0,
        HWND_MESSAGE,  // Message-only window
        nullptr,
        nullptr,
        nullptr
    );

    // Query initial power mode
    SYSTEM_POWER_STATUS sps = {};
    if (GetSystemPowerStatus(&sps)) {
        g_isInPowerSaveMode = (sps.SystemStatusFlag & 1) != 0;
    }
}

void __declspec(dllexport) CleanupPowerModeMonitoring()
{
    if (g_powerModeWindow != nullptr) {
        DestroyWindow(g_powerModeWindow);
        g_powerModeWindow = nullptr;
    }

    UnregisterClassW(POWER_MODE_WINDOW_CLASS, nullptr);

    {
        std::lock_guard<std::mutex> lock(g_callbackMutex);
        g_blurModeCallback = nullptr;
    }
}

void __declspec(dllexport) SetBlurModeChangeCallback(BlurModeCallback callback)
{
    std::lock_guard<std::mutex> lock(g_callbackMutex);
    g_blurModeCallback = callback;

    // Immediately call with current state
    if (callback) {
        callback(g_isInPowerSaveMode);
    }
}