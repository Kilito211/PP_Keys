#ifndef UI_WIN32_H
#define UI_WIN32_H

#include <windows.h>
#include <stdbool.h>

typedef enum
{
    IDC_BTN_START = 1001,
    IDC_EDIT_HOTKEY,
    IDC_BTN_CHANGE_HOTKEY,
    IDC_COUNT,
} idc_t;

bool ui_win32_init(HWND parent);

void ui_win32_deinit(void);

bool ui_win32_process_command(WPARAM wParam, LPARAM lParam);

void ui_win32_set_hotkey_text(const wchar_t *text);

#endif // UI_WIN32_H