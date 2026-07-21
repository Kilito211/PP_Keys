/**
 * @file status_overlay.c
 * @author kilito_hyx (kilito.hyx@gmail.com)
 * @brief 回显悬浮窗
 * @version 0.1
 * @date 2026-07-20
 *
 * @copyright Copyright (c) 2026
 *
 */

#include "status_overlay.h"
#include <stdio.h>

#define STATUS_OVERLAY_CLASS_NAME L"PPKeysStatusOverlay"
#define STATUS_OVERLAY_WIDTH 90
#define STATUS_OVERLAY_HEIGHT 40
#define STATUS_OVERLAY_MARGIN 20

static HWND s_overlay_hwnd = NULL;
static HINSTANCE s_hinstance = NULL;

static LRESULT CALLBACK status_overlay_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

bool status_overlay_init(void)
{
    if (s_overlay_hwnd != NULL)
        return true;

    s_hinstance = GetModuleHandleW(NULL);

    WNDCLASSEXW window_class =
        {
            .cbSize = sizeof(WNDCLASSEXW),
            .lpfnWndProc = status_overlay_proc,
            .hInstance = s_hinstance,
            .hCursor = LoadCursor(NULL, IDC_ARROW),
            .hbrBackground = (HBRUSH)(COLOR_WINDOW + 1),
            .lpszClassName = STATUS_OVERLAY_CLASS_NAME,
        };

    if (!RegisterClassExW(&window_class))
    {
        if (GetLastError() != ERROR_CLASS_ALREADY_EXISTS)
            return false;
    }

    s_overlay_hwnd = CreateWindowExW(WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_LAYERED, STATUS_OVERLAY_CLASS_NAME, L"待机中", WS_POPUP, 0, 0, STATUS_OVERLAY_WIDTH, STATUS_OVERLAY_HEIGHT, NULL, NULL, s_hinstance, NULL);

    if (s_overlay_hwnd == NULL)
        return false;

    SetLayeredWindowAttributes(s_overlay_hwnd, 0, 230, LWA_ALPHA);

    return true;
}

void status_overlay_show(const wchar_t *text)
{
    if (s_overlay_hwnd == NULL)
        return;

    status_overlay_set_text(text);

    if (IsWindowVisible(s_overlay_hwnd))
        return;

    HMONITOR monitor = MonitorFromWindow(s_overlay_hwnd, MONITOR_DEFAULTTONEAREST);

    MONITORINFO monitor_info =
        {
            .cbSize = sizeof(MONITORINFO),
        };

    GetMonitorInfoW(monitor, &monitor_info);

    RECT work_area = monitor_info.rcWork;

    int x = work_area.right - STATUS_OVERLAY_WIDTH - STATUS_OVERLAY_MARGIN;

    int y = work_area.bottom - STATUS_OVERLAY_HEIGHT - STATUS_OVERLAY_MARGIN;

    SetWindowPos(s_overlay_hwnd, HWND_TOPMOST, x, y, STATUS_OVERLAY_WIDTH, STATUS_OVERLAY_HEIGHT, SWP_NOACTIVATE | SWP_SHOWWINDOW);
}

void status_overlay_set_text(const wchar_t *text)
{
    if (s_overlay_hwnd == NULL)
        return;

    SetWindowTextW(s_overlay_hwnd, text);

    InvalidateRect(s_overlay_hwnd, NULL, TRUE);
}

void status_overlay_set_running(bool running)
{
    if (running)
    {
        status_overlay_show(L"按键开启");
    }
    else
    {
        status_overlay_show(L"按键关闭");
    }
}

void status_overlay_hide(void)
{
    if (s_overlay_hwnd != NULL)
    {
        ShowWindow(s_overlay_hwnd, SW_HIDE);
    }
}

void status_overlay_deinit(void)
{
    if (s_overlay_hwnd != NULL)
    {
        DestroyWindow(s_overlay_hwnd);
        s_overlay_hwnd = NULL;
    }

    UnregisterClassW(STATUS_OVERLAY_CLASS_NAME, s_hinstance);
}

static LRESULT CALLBACK status_overlay_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_ERASEBKGND:
        return 1;

    case WM_NCHITTEST:
        return HTTRANSPARENT;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        RECT rect;
        GetClientRect(hwnd, &rect);

        HBRUSH brush = CreateSolidBrush(RGB(40, 40, 40));
        FillRect(hdc, &rect, brush);
        DeleteObject(brush);

        // 设置文字
        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, RGB(255, 255, 255));

        wchar_t text[64];
        GetWindowTextW(hwnd, text, 64);
        DrawTextW(hdc, text, -1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

        EndPaint(hwnd, &ps);
        return 0;
    }

    default:
        return DefWindowProcW(hwnd, msg, wParam, lParam);
    }
}
