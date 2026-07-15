/**
 * @file message_loop.c
 * @author kilito_hyx (kilito.hyx@gmail.com)
 * @brief Win32消息循环分发器
 * @version 0.1
 * @date 2026-07-15
 *
 * @copyright Copyright (c) 2026
 *
 */

#include "message_loop.h"
#include "hotkey.h"
#include <stdio.h>
#include <windows.h>

bool message_loop_dispatch(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_HOTKEY:
        // printf("WM_HOTKEY received, id=%lld\n", wParam);
        return hotkey_process(wParam);
    default:
        return false; // V1.0: 暂时不处理任何事件，后续版本在此处添加Win32相关事件处理
    }
}