/**
 * @file ui.c
 * @author kilito_hyx (kilito.hyx@gmail.com)
 * @brief 通用的UI逻辑
 * @version 0.1
 * @date 2026-07-16
 *
 * @copyright Copyright (c) 2026
 *
 */

#include "ui.h"
#include "ui_win32.h"
#include <windows.h>

/**
 * @brief 更新热键编辑框显示
 * 
 * @param text 显示内容
 */
void ui_set_hotkey_text(const wchar_t *text)
{
    ui_win32_set_hotkey_text(text);
}

