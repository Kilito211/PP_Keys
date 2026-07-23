#ifndef DEVICEIO_WRAPPER_H
#define DEVICEIO_WRAPPER_H

#include <stdbool.h>
#include <stdint.h>
#include <windows.h>

/* IOCTL_KEYBOARD_INSERT_SCANCODE = CTL_CODE(0x0b, 1, 0, 3) = 0x000BC004 */
#define DEVIO_IOCTL_INSERT_SCANCODE 0x000BC004

/*
 * KEYBOARD_INPUT_DATA structure (from ntddkbd.h)
 * Used with IOCTL_KEYBOARD_INSERT_SCANCODE to insert synthetic keystrokes
 */
typedef struct {
    USHORT UnitId;           /* keyboard unit number (0 = first keyboard) */
    USHORT MakeCode;         /* hardware scan code (low byte) */
    USHORT Flags;            /* 0x00=MAKE(press), 0x01=BREAK(release), bit1=E0 */
    USHORT Reserved;         /* must be 0 */
    ULONG  ExtraInformation; /* must be 0 */
} DEVIO_KEYBOARD_INPUT_DATA;

/* Stroke flags */
#define DEVIO_KEY_MAKE  0
#define DEVIO_KEY_BREAK 1
#define DEVIO_KEY_E0    2

/* Public API */
bool deviceio_wrapper_init(void);
bool deviceio_wrapper_key_down(uint16_t vk);
bool deviceio_wrapper_key_up(uint16_t vk);
void deviceio_wrapper_deinit(void);

#endif /* DEVICEIO_WRAPPER_H */
