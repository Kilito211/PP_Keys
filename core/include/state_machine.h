#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include "app_event.h"
#include <stdbool.h>

typedef enum
{
    APP_STATE_IDLE = 0,
    APP_STATE_RUNNING,
} app_state_t;

bool state_machine_init(void);

void state_machine_handle_event(app_event_t event);

app_state_t state_machine_get_state(void);

void state_machine_deinit(void);

#endif //STATE_MACHINEP_H