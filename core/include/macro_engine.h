#ifndef MACRO_ENGINE_H
#define MACRO_ENGINE_H

#include <stdbool.h>
#include <stdint.h>

bool macro_engine_init(void);

bool macro_engine_start(void);

void macro_engine_stop(void);

bool macro_engine_is_running(void);

void macro_engine_deinit(void);

#endif //MACRO_ENGINE_H