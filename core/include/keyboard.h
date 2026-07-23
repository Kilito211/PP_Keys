#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdbool.h>
#include <stdint.h>
#include <windows.h>

typedef enum
{
    INPUT_SENDINPUT_VK = 0,
    INPUT_SENDINPUT_SCANCODE,
    INPUT_KEYBD_EVENT,
    INPUT_NT_SENDINPUT,
    INPUT_INTERCEPTION,  // Interception 驱动 (kbdclass 层过滤驱动,需安装驱动)
    INPUT_DEVICEIO,      // DeviceIoControl -> \\.\\KEYBOARD\\0 (需管理员权限,免装驱动)
} input_mode_t;

bool keyboard_init(void);
bool keyboard_send(uint16_t key, DWORD flags);
bool keyboard_key_down(uint16_t key);
bool keyboard_key_up(uint16_t key);
bool keyboard_press(uint16_t key);
void keyboard_set_mode(input_mode_t mode);
input_mode_t keyboard_get_mode(void);
const wchar_t *keyboard_get_mode_name(input_mode_t mode);
void keyboard_deinit(void);

#endif