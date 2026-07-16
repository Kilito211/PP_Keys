#ifndef HOTKEY_H
#define HOTKEY_H

#include <stdbool.h>
#include <stdint.h>
#include <windows.h>

typedef enum
{
    HOTKEY_ID_NONE = 0,
    HOTKEY_ID_START, // 启停键
    HOTKEY_ID_MAX,
} hotkey_id_t;

typedef struct
{
    bool registered;
    hotkey_id_t id;

} hotkey_t;

bool hotkey_init(void);

bool hotkey_start_listen(HWND hwnd);

void hotkey_stop_listen(void);

bool hotkey_process(unsigned int id);

void hotkey_deinit(void);

bool hotkey_set(uint16_t key);

uint16_t hotkey_get(void);

#endif //HOTKEY_H