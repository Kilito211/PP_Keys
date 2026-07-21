#ifndef CONFIG_H
#define CONFIG_H

#include <stdbool.h>
#include <stdint.h>

typedef struct
{
    uint32_t magic;
    uint32_t version;
    uint16_t hotkey;
    uint16_t action_count;
}config_header_t;


bool config_init(void);

bool config_save(void);

void config_deinit(void);

#endif //CONFIG_H
