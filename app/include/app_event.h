#ifndef APP_EVENT_H
#define APP_EVENT_H

#include <stdbool.h>

typedef enum
{
    APP_EVENT_NONE = 0,
    APP_EVENT_START,
    APP_EVENT_STOP,
    APP_EVENT_EXIT,
    APP_EVENT_MAX,
} app_event_t;

bool app_event_init(void);

void app_event_post(app_event_t event);

void app_event_deinit(void);

#endif // APP_EVENT_H