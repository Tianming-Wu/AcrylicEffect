#include "acrylicEffect_private.h"
#include <stdexcept>
#include <comdef.h>

HMODULE dwmapi = nullptr;

BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD fdwReason, LPVOID lpReserved)
{
    switch(fdwReason) 
    { 
        case DLL_PROCESS_ATTACH:
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
            if (lpReserved != nullptr) break; // do not do cleanup if process termination scenario
            #ifndef _MSC_VER
            FreeLibrary(dwmapi);
            #endif
        
            break;
    }
    return TRUE;  // Successful DLL_PROCESS_ATTACH.
}

void __declspec(dllexport) SetAcrylicEffect(HWND hwnd, DWM_SYSTEMBACKDROP_TYPE type)
{
    constexpr auto margins = MARGINS{-1, -1, -1, -1}; //若值其中某一值为负数，则会扩展到整个窗口，正好满足需求
    DwmSetWindowAttribute(hwnd, DWMWA_SYSTEMBACKDROP_TYPE, &type, sizeof(type));
}

bool __declspec(dllexport) SetBlurEffect(HWND hwnd, Region region)
{
    if(hwnd == nullptr) throw std::invalid_argument("hwnd is null");

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

HRESULT __declspec(dllexport) SetBlurEffectHr(HWND hwnd, Region region) {
    if (hwnd == nullptr || !IsWindow(hwnd))
        return HRESULT_FROM_WIN32(ERROR_INVALID_HANDLE);

    HRGN h = region.handle();
    if (h == nullptr || GetObjectType(h) != OBJ_REGION)
        return HRESULT_FROM_WIN32(ERROR_INVALID_PARAMETER);

    DWM_BLURBEHIND bb = {};
    bb.dwFlags = DWM_BB_ENABLE | DWM_BB_BLURREGION;
    bb.fEnable = TRUE;
    bb.hRgnBlur = h;

    return DwmEnableBlurBehindWindow(hwnd, &bb); // 返回 HRESULT 供外部解码
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

Region::Region(Region&& other) noexcept : hRgn(other.hRgn) {
    other.hRgn = nullptr;
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

bool __declspec(dllexport) SetMicaEffect(HWND hwnd, DWM_SYSTEMBACKDROP_TYPE type, DWM_WINDOW_CORNER_PREFERENCE corner)
{
    if(!hwnd) return false;

    DWM_SYSTEMBACKDROP_TYPE backdrop = type;

    HRESULT hr1 = DwmSetWindowAttribute(hwnd, DWMWA_SYSTEMBACKDROP_TYPE, &backdrop, sizeof(backdrop));

    // 设置圆角偏好（与 Qt 圆角绘制搭配）
    DWM_WINDOW_CORNER_PREFERENCE cp = corner;
    HRESULT hr2 = DwmSetWindowAttribute(hwnd, DWMWA_WINDOW_CORNER_PREFERENCE, &cp, sizeof(cp));

    return SUCCEEDED(hr1) && SUCCEEDED(hr2);
}