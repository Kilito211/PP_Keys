#ifndef UI_WIN32_H
#define UI_WIN32_H

#include "action_list.h"
#include "keyboard.h"
#include <windows.h>
#include <stdbool.h>

#define UI_MARGIN_X 20

#define UI_HOTKEY_Y 20
#define UI_ACTION_TITLE_Y 80
#define UI_ADD_Y 110
#define UI_INPUT_KEY_X 110
#define UI_INPUT_DELAY_X 260
#define UI_TABLE_Y 150
#define UI_ROW_HEIGHT 25

typedef enum
{
    IDC_BTN_START = 3001,
    IDC_BTN_CHANGE_HOTKEY,
    IDC_BTN_ADD,
    IDC_BTN_ACTION_KEY,
    IDC_EDIT_ACTION_DELAY,
    IDC_COMBO_INPUT_MODE = 4000,
    IDC_BTN_DELETE_BASE = 5000,
    IDC_COUNT,
} idc_t;

typedef struct
{
    macro_action_t data;
    HWND key;
    HWND delay;
    HWND delete_btn;
} ui_action_item_t;

bool ui_win32_init(HWND parent);
void ui_win32_deinit(void);
bool ui_win32_process_command(WPARAM wParam, LPARAM lParam);
void ui_win32_set_hotkey_text(const wchar_t *text);
bool ui_win32_refresh_action_list(void);
void ui_win32_set_action_key_text(const wchar_t *text);
void ui_win32_set_action_key(uint16_t vk);
void ui_win32_update_state(void);

#endif
HWND ui_win32_get_table_frame(void);
