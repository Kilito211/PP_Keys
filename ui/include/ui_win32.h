#ifndef UI_WIN32_H
#define UI_WIN32_H

#include "action_list.h"
#include <windows.h>
#include <stdbool.h>

#define UI_MARGIN_X 20 // 页边距

#define UI_HOTKEY_Y 20 // 热键纵坐标

#define UI_ACTION_TITLE_Y 80 // ACTION标题纵坐标

#define UI_ADD_Y 110 // ADD按键纵坐标

#define UI_INPUT_KEY_X 110   // 添加键值横坐标
#define UI_INPUT_DELAY_X 260 // 添加DELAY横坐标

#define UI_TABLE_Y 150   // 表格纵坐标
#define UI_ROW_HEIGHT 25 // 行高

typedef enum
{
    IDC_BTN_START = 3001,
    IDC_BTN_CHANGE_HOTKEY,
    IDC_BTN_ADD,
    IDC_BTN_ACTION_KEY,
    IDC_EDIT_ACTION_DELAY,
    IDC_BTN_DELETE_BASE = 5000,
    IDC_COUNT,
} idc_t;

typedef struct
{
    macro_action_t data; // 动作数据
    HWND key;            // 按键控件
    HWND delay;          // 延时数据显示控件
    HWND delete_btn;     // 删除按键

} ui_action_item_t;

bool ui_win32_init(HWND parent);

void ui_win32_deinit(void);

bool ui_win32_process_command(WPARAM wParam, LPARAM lParam);

void ui_win32_set_hotkey_text(const wchar_t *text);

bool ui_win32_refresh_action_list(void);

void ui_win32_set_action_key_text(const wchar_t *text);

void ui_win32_set_action_key(uint16_t vk);

void ui_win32_update_state(void);

#endif // UI_WIN32_H