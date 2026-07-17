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
#include "ui_capture.h"
#include <stdio.h>
#include <windows.h>

bool message_loop_dispatch(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_HOTKEY:
        return hotkey_process(wParam);
    case WM_KEYDOWN:
        uint16_t vk = (uint16_t)wParam;

        if (ui_capture_is_active())
        {
            if (vk == VK_PROCESSKEY)
            {
                BYTE key_state[256];
                if (GetKeyboardState(key_state))
                {
                    for (int i = 0x41; i <= 0x5A; i++)
                    {
                        if (key_state[i] & 0x80)
                        {
                            return ui_capture_process((uint16_t)i);
                        }
                    }
                    for (int i = 0x30; i <= 0x39; i++)
                    {
                        if (key_state[i] & 0x80)
                        {
                            return ui_capture_process((uint16_t)i);
                        }
                    }
                }
                return false;
            }

            if (vk > 0 && vk <= 0xFE)
            {
                return ui_capture_process(vk);
            }
        }
        break;
    default:
        return false;
    }
}