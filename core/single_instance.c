/**
 * @file single_instance.c
 * @author kilito_hyx (kilito.hyx@gmail.com)
 * @brief 简易实例锁，在已有进程时不创建第二个进程
 * @version 0.1
 * @date 2026-07-17
 *
 * @copyright Copyright (c) 2026
 *
 */
#include "single_instance.h"

#define PPKEYS_MUTEX_NAME L"PPKeys_Single_Instance"
#define WM_SHOW_WINDOW (WM_USER + 10)

static HANDLE s_mutex = NULL;

bool single_instance_check(void)
{
    s_mutex = CreateMutexW(
        NULL,
        TRUE,
        PPKEYS_MUTEX_NAME);

    if (GetLastError() == ERROR_ALREADY_EXISTS)
    {
        HWND hwnd = FindWindowW(
            L"PPKeysWindowClass",
            NULL);

        if (hwnd)
        {
            PostMessageW(
                hwnd,
                WM_SHOW_WINDOW,
                0,
                0);
        }

        return false;
    }

    return true;
}