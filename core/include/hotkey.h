#ifndef HOTKEY_H
#define HOTKEY_H

#include <stdbool.h>

typedef enum
{
    HOTKEY_ID_NONE = 0,
    HOTKEY_ID_START, // 启停键
    HOTKEY_ID_MAX,
} hotkey_id_t;

bool hotkey_init(void);

bool hotkey_register(hotkey_id_t id, unsigned int vk);

void hotkey_unregister(hotkey_id_t id);

bool hotkey_process(unsigned int id);

void hotkey_deinit(void);

#endif //HOTKEY_H