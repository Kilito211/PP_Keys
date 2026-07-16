/**
 * @file hotkey.c
 * @author kilito_hyx (kilito.hyx@gmail.com)
 * @brief 注销、修改热键
 * @version 0.1
 * @date 2026-07-14
 *
 * @copyright Copyright (c) 2026
 *
 */

#include "hotkey.h"
#include "app_event.h"
#include <stdio.h>
#include <windows.h>

static HWND s_hWnd = NULL;

/**
 * @brief 初始化热键模块
 *
 * @return true 初始化成功
 * @return false 初始化失败
 */
bool hotkey_init(void)
{
    // return hotkey_start_listen(window_get_handle());
    return true;
}

/**
 * @brief 开始监听热键
 *
 * @return true 成功
 * @return false 失败
 */
bool hotkey_start_listen(HWND hwnd)
{
    s_hWnd = hwnd;

    if (!RegisterHotKey(s_hWnd, HOTKEY_ID_START, 0, VK_F8)) // 注册热键， NULL：系统级热键
    {
        printf("RegisterHotKey failed, error=%lu\n", GetLastError());
        return false;
    }
    printf("RegisterHotKey success\n");
    return true;
}

/**
 * @brief 停止监听热键
 *
 */
void hotkey_stop_listen(void)
{
    if (s_hWnd)
        UnregisterHotKey(s_hWnd, HOTKEY_ID_START);
}

/**
 * @brief 处理热键事件
 *
 * @param id 热键ID
 * @return true 已处理
 * @return false 未处理
 */
bool hotkey_process(unsigned int id)
{
    switch (id)
    {
    case HOTKEY_ID_START:
        /* 下一节发送APP_EVENT */
        app_event_post(APP_EVENT_START);
        return true;
    default:
        return false;
    }
}

/**
 * @brief 反初始化热键模块
 *
 */
void hotkey_deinit(void)
{
    // hotkey_stop_listen();
}