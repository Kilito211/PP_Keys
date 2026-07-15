#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdbool.h>
#include <stdint.h>
#include <windows.h>

bool keyboard_init(void);

static bool keyboard_send(uint16_t key, DWORD flags);

bool keyboard_key_down(uint16_t key);

bool keyboard_key_up(uint16_t key);

bool keyboard_press(uint16_t key);

void keyboard_deinit(void);

#endif //KEYBOARD_H