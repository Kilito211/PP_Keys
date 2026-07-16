/**
 * @file ui_win32.c
 * @author kilito_hyx (kilito.hyx@gmail.com)
 * @brief Win32除主窗口以外的UI
 * @version 0.1
 * @date 2026-07-16
 *
 * @copyright Copyright (c) 2026
 *
 */

#include "ui_win32.h"
#include "app_event.h"
#include <stdio.h>
#include <stdint.h>

static HWND s_btn_start = NULL;

/**
 * @brief 初始化UI
 *
 * @param parent 父窗口句柄
 * @return true 成功
 * @return false 失败
 */
bool ui_win32_init(HWND parent)
{
    s_btn_start = CreateWindowExW(0, L"BUTTON", L"Start/Stop", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 20, 20, 140, 40, parent, (HMENU)IDC_BTN_START, GetModuleHandleW(NULL), NULL);

    if (s_btn_start == NULL)
        return false;
    return true;
}

/**
 * @brief 按键事件分发
 *
 * @param wParam 低16位为控件ID，高16位为通知码
 * @param lParam 参数2
 * @return true
 * @return false
 */
bool ui_win32_process_command(WPARAM wParam, LPARAM lParam)
{
    uint16_t id = LOWORD(wParam);
    uint16_t notify = HIWORD(wParam);
    switch (id)
    {
    case IDC_BTN_START:
        app_event_post(APP_EVENT_START);
        return true;
    default:
        return false;
    }
}

/**
 * @brief 析构UI
 *
 */
void ui_win32_deinit(void)
{
}