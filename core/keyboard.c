/**
 * @file keyboard.c
 * @author kilito_hyx (kilito.hyx@gmail.com)
 * @brief Windows键盘模拟
 * @version 0.1
 * @date 2026-07-15
 *
 * @copyright Copyright (c) 2026
 *
 */

#include "keyboard.h"
#include <windows.h>

/**
 * @brief 键盘模拟初始化
 *
 * @return true 成功
 * @return false 失败
 */
bool keyboard_init(void)
{
    return true;
}

/**
 * @brief 发送键盘事件
 *
 * @param key 键值
 * @param flags 事件标志
 * @return true 成功
 * @return false 失败
 */
static bool keyboard_send(uint16_t key, DWORD flags)
{
    INPUT input = {0};
    input.type = INPUT_KEYBOARD;
    input.ki.wVk = key;
    input.ki.dwFlags = flags;

    UINT result = SendInput(1, &input, sizeof(INPUT));

    return result == 1;
}

/**
 * @brief 按下按键
 *
 * @param key 键值
 * @return true 成功
 * @return false 失败
 */
bool keyboard_key_down(uint16_t key)
{
    return keyboard_send(key, 0);
}

/**
 * @brief 松开按键
 *
 * @param key 键值
 * @return true 成功
 * @return false 失败
 */
bool keyboard_key_up(uint16_t key)
{
    return keyboard_send(key, KEYEVENTF_KEYUP);
}

/**
 * @brief 完整按下并松开按键
 *
 * @param key 键值
 * @return true 成功
 * @return false 失败
 */
bool keyboard_press(uint16_t key)
{
    if (!keyboard_key_down(key))
        return false;
    // 中间可以加入延迟
    return keyboard_key_up(key);
}

/**
 * @brief 析构键盘模块
 *
 */
void keyboard_deinit(void)
{
}