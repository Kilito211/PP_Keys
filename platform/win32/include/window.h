#ifndef WINDOW_H
#define WINDOW_H

#include <stdbool.h>
#include <windows.h>

bool window_init(void);

void window_show(void);

void window_run(void);

void window_deinit(void);

HWND window_get_handle(void);

#endif // _WINDOW_H_