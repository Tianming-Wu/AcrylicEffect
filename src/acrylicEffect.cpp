#include "acrylicEffect_private.h"

HMODULE dwmapi = nullptr;

BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD fdwReason, LPVOID lpReserved)
{
    switch(fdwReason) 
    { 
        case DLL_PROCESS_ATTACH:
            #ifndef _MSC_VER
            dwmapi = LoadLibrary(TEXT("dwmapi.dll"));
            const auto pfn_ExtendFrameIntoClientArea = GetProcAddress(dwmapi, "DwmExtendFrameIntoClientArea")
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