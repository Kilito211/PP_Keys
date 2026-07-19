#ifndef TRAY_H
#define TRAY_H

#include <windows.h>
#include <stdbool.h>

#define WM_TRAY_NOTIFY (WM_USER + 1)

#define ID_TRAY_SHOW 1001
#define ID_TRAY_EXIT 1002

bool tray_init(HWND hwnd);



bool tray_process_message(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

void tray_remove(void);

#endif // TRAY_H