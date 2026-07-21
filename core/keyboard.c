/**
 * @file keyboard.c
 * @brief Windows键盘模拟，支持多种输入模式
 */

#include "keyboard.h"
#include <stdio.h>

static input_mode_t s_input_mode = INPUT_SENDINPUT_VK;

bool keyboard_init(void) { return true; }

void keyboard_set_mode(input_mode_t mode)
{
    if (mode >= INPUT_SENDINPUT_VK && mode <= INPUT_KEYBD_EVENT)
    {
        s_input_mode = mode;
        printf("Keyboard: mode -> %ls\n", keyboard_get_mode_name(mode));
    }
}

input_mode_t keyboard_get_mode(void) { return s_input_mode; }

const wchar_t *keyboard_get_mode_name(input_mode_t mode)
{
    switch (mode)
    {
    case INPUT_SENDINPUT_VK:       return L"SendInput-\u865A\u62DF\u952E\u7801";
    case INPUT_SENDINPUT_SCANCODE: return L"SendInput-\u626B\u63CF\u7801";
    case INPUT_KEYBD_EVENT:        return L"keybd_event";
    default:                       return L"\u672A\u77E5";
    }
}

bool keyboard_send(uint16_t key, DWORD flags)
{
    switch (s_input_mode)
    {
    case INPUT_SENDINPUT_VK:
    {
        INPUT input = {0};
        input.type = INPUT_KEYBOARD;
        input.ki.wVk = key;
        input.ki.dwFlags = flags;
        return SendInput(1, &input, sizeof(INPUT)) == 1;
    }
    case INPUT_SENDINPUT_SCANCODE:
    {
        INPUT input = {0};
        input.type = INPUT_KEYBOARD;
        input.ki.wScan = (WORD)MapVirtualKeyW(key, MAPVK_VK_TO_VSC);
        input.ki.dwFlags = flags | KEYEVENTF_SCANCODE;
        return SendInput(1, &input, sizeof(INPUT)) == 1;
    }
    case INPUT_KEYBD_EVENT:
    {
        BYTE vk = (BYTE)key;
        keybd_event(vk, MapVirtualKeyW(vk, MAPVK_VK_TO_VSC),
                    (flags & KEYEVENTF_KEYUP) ? KEYEVENTF_KEYUP : 0, 0);
        return true;
    }
    default:
        return false;
    }
}

bool keyboard_key_down(uint16_t key) { return keyboard_send(key, 0); }
bool keyboard_key_up(uint16_t key)   { return keyboard_send(key, KEYEVENTF_KEYUP); }

bool keyboard_press(uint16_t key)
{
    if (!keyboard_send(key, 0)) return false;
    Sleep(15);
    return keyboard_send(key, KEYEVENTF_KEYUP);
}

void keyboard_deinit(void) {}