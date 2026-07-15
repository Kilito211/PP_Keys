#ifndef MESSAGE_LOOP_H
#define MESSAGE_LOOP_H

#include <stdbool.h>
#include <windows.h>

bool message_loop_dispatch(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);

#endif //MESSAGE_LOOP_H