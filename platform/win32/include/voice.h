#ifndef VOICE_H
#define VOICE_H

#include <stdbool.h>
#include <wchar.h>
#include <windows.h>

bool voice_init(void);

bool voice_speak(const wchar_t *text);

void voice_deinit(void);

#endif //VOICE_H