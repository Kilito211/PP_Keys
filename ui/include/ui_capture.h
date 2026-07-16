#ifndef UI_CAPTURE_H
#define UI_CAPTURE_H

#include <stdbool.h>
#include <stdint.h>

typedef enum
{
    UI_CAPTURE_NONE = 0,

    UI_CAPTURE_HOTKEY,

    UI_CAPTURE_ACTION_KEY,

} ui_capture_mode_t;

bool ui_capture_init(void);

void ui_capture_begin(ui_capture_mode_t mode);

void ui_capture_end(void);

bool ui_capture_is_active(void);

ui_capture_mode_t ui_capture_get_mode(void);

bool ui_capture_process(uint16_t vk);

void ui_capture_deinit(void);

#endif