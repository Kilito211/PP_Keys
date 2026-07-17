/**
 * @file tray.c
 * @author kilito_hyx (kilito.hyx@gmail.com)
 * @brief 托盘资源管理
 * @version 0.1
 * @date 2026-07-17
 *
 * @copyright Copyright (c) 2026
 *
 */

#include "tray.h"
#include <windows.h>
#include <shellapi.h>
#include "resource.h"

static NOTIFYICONDATAW s_notify = {0};

/**
 * @brief 托盘初始化
 *
 * @param hwnd 窗口句柄
 * @return true 成功
 * @return false 失败
 */
bool tray_init(HWND hwnd)
{
    s_notify.cbSize = sizeof(NOTIFYICONDATAW);

    s_notify.hWnd = hwnd;

    s_notify.uID = 1;

    s_notify.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;

    s_notify.uCallbackMessage = WM_TRAY_NOTIFY;

    s_notify.hIcon = LoadIconW(GetModuleHandleW(NULL), MAKEINTRESOURCEW(IDI_ICON1));
    // s_notify.hIcon = LoadIconW(NULL, (LPCWSTR)IDI_APPLICATION);

    wcscpy_s(s_notify.szTip, 128, L"PPKeys");

    return Shell_NotifyIconW(NIM_ADD, &s_notify);
}

/**
 * @brief 移除托盘
 *
 */
void tray_remove(void)
{
    Shell_NotifyIconW(NIM_DELETE, &s_notify);
}

/**
 * @brief 托盘右键菜单
 *
 * @param hwnd 窗口句柄
 */
static void tray_show_menu(HWND hwnd)
{
    POINT pt;

    GetCursorPos(&pt);
    HMENU menu = CreatePopupMenu();

    AppendMenuW(menu, MF_STRING, ID_TRAY_SHOW, L"显示窗口");

    AppendMenuW(menu, MF_SEPARATOR, 0, NULL);

    AppendMenuW(menu, MF_STRING, ID_TRAY_EXIT, L"退出");

    SetForegroundWindow(hwnd);

    TrackPopupMenu(menu, TPM_RIGHTBUTTON, pt.x, pt.y, 0, hwnd, NULL);

    DestroyMenu(menu);
}

/**
 * @brief 托盘事件处理
 *
 * @param hwnd 窗口句柄
 * @param msg 事件类型
 * @param wParam 消息1
 * @param lParam 消息2
 * @return true
 * @return false
 */
bool tray_process_message(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg == WM_TRAY_NOTIFY)
    {
        switch (lParam)
        {
        case WM_LBUTTONDBLCLK:
            ShowWindow(hwnd, SW_SHOW);
            SetForegroundWindow(hwnd);
            return true;
        case WM_RBUTTONUP:
            tray_show_menu(hwnd);
            return true;
        }
    }

    if (msg == WM_COMMAND)
    {
        switch (LOWORD(wParam))
        {
        case ID_TRAY_SHOW:
            ShowWindow(hwnd, SW_SHOW);
            SetForegroundWindow(hwnd);
            return true;
        case ID_TRAY_EXIT:
            DestroyWindow(hwnd);
            return true;
        }
    }
    return false;
}