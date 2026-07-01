#include "visualEffect.h"
#include <windows.h>
#include <cstdio>

// Old API types
enum ACCENT_STATE {
    ACCENT_DISABLED = 0,
    ACCENT_ENABLE_GRADIENT = 1,
    ACCENT_ENABLE_TRANSPARENTGRADIENT = 2,
    ACCENT_ENABLE_BLURBEHIND = 3,
    ACCENT_ENABLE_ACRYLICBLURBEHIND = 4,
};
struct ACCENT_POLICY {
    ACCENT_STATE AccentState;
    DWORD AccentFlags;
    DWORD GradientColor;
    DWORD AnimationId;
};
struct WINDOWCOMPOSITIONATTRIBDATA {
    DWORD Attrib; PVOID pvData; SIZE_T cbData;
};
enum WCA { WCA_ACCENT_POLICY = 19 };
using pSetWindowCompositionAttribute = BOOL(WINAPI*)(HWND, WINDOWCOMPOSITIONATTRIBDATA*);

static bool SetAccent(HWND hwnd, ACCENT_STATE state)
{
    HMODULE hUser = LoadLibraryW(L"user32.dll");
    if (!hUser) return false;
    auto fn = (pSetWindowCompositionAttribute)GetProcAddress(hUser, "SetWindowCompositionAttribute");
    if (!fn) return false;
    ACCENT_POLICY policy = {};
    policy.AccentState = state;
    WINDOWCOMPOSITIONATTRIBDATA data = { WCA_ACCENT_POLICY, &policy, sizeof(policy) };
    return fn(hwnd, &data) != FALSE;
}

LRESULT CALLBACK BlurWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK ContentWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

const wchar_t BLUR_WND_CLASS[]    = L"WveBlurWindow";
const wchar_t CONTENT_WND_CLASS[] = L"WveContentWindow";

int main()
{
    HINSTANCE hInstance = GetModuleHandle(nullptr);

    // Register blur window class
    WNDCLASSW wc = {};
    wc.lpfnWndProc   = BlurWndProc;
    wc.hInstance     = hInstance;
    wc.hCursor       = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = CreateSolidBrush(RGB(255, 255, 255));
    wc.lpszClassName = BLUR_WND_CLASS;
    RegisterClassW(&wc);

    // Register content window class (no background, just click surface)
    WNDCLASSW cc = {};
    cc.lpfnWndProc   = ContentWndProc;
    cc.hInstance     = hInstance;
    cc.hCursor       = LoadCursor(nullptr, IDC_ARROW);
    cc.hbrBackground = nullptr;
    cc.lpszClassName = CONTENT_WND_CLASS;
    RegisterClassW(&cc);

    // --- 1. Blur window (back) — draws content ---
    HWND hBlur = CreateWindowExW(
        WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_NOACTIVATE,
        BLUR_WND_CLASS, L"WVE Blur",
        WS_POPUP,
        200, 200, 800, 600,
        nullptr, nullptr, hInstance, nullptr
    );
    if (!hBlur) return 1;

    SetLayeredWindowAttributes(hBlur, RGB(255, 255, 255), 255, LWA_COLORKEY);
    SetAcrylicEffect(hBlur, EffectType_Acrylic, Corner_Round);
    SetAccent(hBlur, ACCENT_ENABLE_BLURBEHIND);

    // --- 2. Content window (front) — click catcher, no drawing ---
    HWND hContent = CreateWindowExW(
        WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_NOACTIVATE,
        CONTENT_WND_CLASS, nullptr,
        WS_POPUP,
        200, 200, 800, 600,
        nullptr, nullptr, hInstance, nullptr
    );
    if (!hContent) return 1;

    SetLayeredWindowAttributes(hContent, 0, 1, LWA_ALPHA);

    // Sync Z-order: blur behind content
    SetWindowPos(hBlur, hContent, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

    ShowWindow(hBlur, SW_SHOWNORMAL);
    ShowWindow(hContent, SW_SHOWNORMAL);
    UpdateWindow(hBlur);
    UpdateWindow(hContent);

    printf("Running. Press ESC to exit.\n");

    MSG msg = {};
    while (GetMessageW(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    DeleteObject(wc.hbrBackground);
    return 0;
}

// Blur window: draws content + blur
LRESULT CALLBACK BlurWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg == WM_DESTROY)
    {
        // Close content window too
        HWND hContent = FindWindowW(CONTENT_WND_CLASS, nullptr);
        if (hContent) DestroyWindow(hContent);
        PostQuitMessage(0);
        return 0;
    }
    switch (msg)
    {
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        RECT client;
        GetClientRect(hwnd, &client);

        // Border
        HPEN hPen = CreatePen(PS_INSIDEFRAME, 1, RGB(0, 0, 0));
        HGDIOBJ hOldPen = SelectObject(hdc, hPen);
        HGDIOBJ hOldBrush = SelectObject(hdc, GetStockObject(NULL_BRUSH));
        Rectangle(hdc, 0, 0, client.right, client.bottom);
        SelectObject(hdc, hOldBrush);
        SelectObject(hdc, hOldPen);
        DeleteObject(hPen);

        // Text
        HFONT hFont = CreateFontW(19, 0, 0, 0,
            FW_MEDIUM, FALSE, FALSE, 0, DEFAULT_CHARSET,
            OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
            NONANTIALIASED_QUALITY, DEFAULT_PITCH, L"Segoe UI");
        HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);

        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, RGB(0, 0, 0));

        RECT textRect = client;
        textRect.left += 30;
        textRect.top += 30;
        DrawTextW(hdc, L"Hello from WindowsVisualEffect!", -1, &textRect,
            DT_LEFT | DT_TOP | DT_WORDBREAK);

        SelectObject(hdc, hOldFont);
        DeleteObject(hFont);
        EndPaint(hwnd, &ps);
        return 0;
    }
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

// Content window: transparent click catcher, no drawing
LRESULT CALLBACK ContentWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_DESTROY:
        return 0;
    case WM_KEYDOWN:
        if (wParam == VK_ESCAPE)
        {
            HWND hBlur = FindWindowW(BLUR_WND_CLASS, nullptr);
            if (hBlur) DestroyWindow(hBlur);
            DestroyWindow(hwnd);
        }
        return 0;
    case WM_LBUTTONDOWN:
        SendMessageW(hwnd, WM_NCLBUTTONDOWN, HTCAPTION, lParam);
        return 0;
    case WM_ERASEBKGND:
        return 1;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        BeginPaint(hwnd, &ps);  // validate region, no drawing
        EndPaint(hwnd, &ps);
        return 0;
    }
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}
