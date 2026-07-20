#ifndef STATUS_OVERLAY
#define STATUS_OVERLAY

#include <stdbool.h>
#include <windows.h>

bool status_overlay_init(void);

void status_overlay_show(const wchar_t *text);

void status_overlay_set_text(const wchar_t *text);

void status_overlay_set_running(bool running);

void status_overlay_hide(void);

void status_overlay_deinit(void);

#endif // STATUS_OVERLAY