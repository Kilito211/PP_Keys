/**
 * @file ui_capture.c
 * @author kilito_hyx (kilito.hyx@gmail.com)
 * @brief UI添加按键时的捕获
 * @version 0.1
 * @date 2026-07-16
 *
 * @copyright Copyright (c) 2026
 *
 */

#include "ui_capture.h"
#include "ui.h"
#include "hotkey.h"
#include "window.h"
#include "ui_win32.h"
#include <stdio.h>
#include <windows.h>

static ui_capture_mode_t s_mode = UI_CAPTURE_NONE;

/**
 * @brief 初始化捕获
 *
 * @return true 成功
 * @return false 失败
 */
bool ui_capture_init(void)
{
    s_mode = UI_CAPTURE_NONE;
    return true;
}

/**
 * @brief 开始捕获
 *
 * @param mode 捕获的按键类型
 */
void ui_capture_begin(ui_capture_mode_t mode)
{
    s_mode = mode;
    switch (mode)
    {
    case UI_CAPTURE_HOTKEY:
        ui_set_hotkey_text(L"Press key");
        break;

    case UI_CAPTURE_ACTION_KEY:
        ui_win32_set_action_key_text(L"Press key");
        break;

    default:
        break;
    }
    SetFocus(window_get_handle());
}

/**
 * @brief 结束捕获
 *
 */
void ui_capture_end(void)
{
    s_mode = UI_CAPTURE_NONE;
}

/**
 * @brief 是否正在捕获
 *
 * @return true
 * @return false
 */
bool ui_capture_is_active(void)
{
    return s_mode != UI_CAPTURE_NONE;
}

/**
 * @brief 正在捕获的按键类型
 *
 * @return ui_capture_mode_t 类型
 */
ui_capture_mode_t ui_capture_get_mode(void)
{
    return s_mode;
}

/**
 * @brief 获取当前捕获的键值
 *
 * @param vk
 * @return true 成功
 * @return false 失败
 */
bool ui_capture_process(uint16_t vk)
{
    wchar_t text[64];

    if (vk == 0 || vk > 0xFE)
        return false;

    if (!ui_capture_is_active())
        return false;

    switch (s_mode)
    {
    case UI_CAPTURE_HOTKEY:
        hotkey_set(vk);
        hotkey_get_name(vk, text, 32);
        ui_set_hotkey_text(text);
        break;
    case UI_CAPTURE_ACTION_KEY:
        ui_win32_set_action_key(vk);
        break;
    default:
        break;
    }
    ui_capture_end();

    return true;
}

void ui_capture_deinit(void)
{
}