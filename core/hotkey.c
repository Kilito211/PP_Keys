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
static uint16_t s_hotkey = VK_F8;

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

    if (!RegisterHotKey(s_hWnd, HOTKEY_ID_START, 0, s_hotkey)) // 注册热键
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

/**
 * @brief 自定义热键接口
 * 
 * @param key 键值
 * @return true 成功
 * @return false 失败
 */
bool hotkey_set(uint16_t key)
{
    hotkey_stop_listen();
    s_hotkey = key;
    hotkey_start_listen(s_hWnd);
    return true;
}

/**
 * @brief 获取当前热键
 * 
 * @return uint16_t 当前热键键值
 */
uint16_t hotkey_get(void)
{
    return s_hotkey;
}

/**
 * @brief 获取热键名称
 * 
 * @param vk 键值
 * @param text 名称文本
 * @param len 长度
 * @return true 成功
 * @return false 失败
 */
bool hotkey_get_name(uint16_t vk,wchar_t *text,uint32_t len)
{
    if(text == NULL || len == 0)
        return false;

    UINT scan = MapVirtualKey(vk, MAPVK_VK_TO_VSC);

    LONG lParam = scan << 16;
    if(GetKeyNameTextW(lParam,text,len) == 0)
        swprintf(text, len, L"%u", vk);
    return true;
}