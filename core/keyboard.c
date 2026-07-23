/**
 * @file keyboard.c
 * @author kilito_hyx (kilito.hyx@gmail.com)
 * @brief Windows键盘模拟,支持多种输入模式
 * @version 0.1
 * @date 2026-07-15
 *
 * @copyright Copyright (c) 2026
 *
 */

#include "keyboard.h"
#include "interception_wrapper.h"
#include "deviceio_wrapper.h"
#include <stdio.h>

typedef LONG (WINAPI *fn_NtUserSendInput_t)(UINT, LPINPUT, int);
static fn_NtUserSendInput_t s_NtUserSendInput = NULL;

static bool keyboard_load_nt_sendinput(void)
{
    if (s_NtUserSendInput)
        return true;
    HMODULE hWin32u = GetModuleHandleW(L"win32u.dll");
    if (hWin32u)
    {
        s_NtUserSendInput = (fn_NtUserSendInput_t)GetProcAddress(hWin32u, "NtUserSendInput");
        if (s_NtUserSendInput)
        {
            printf("Keyboard: NtUserSendInput loaded\n");
            return true;
        }
    }
    printf("Keyboard: NtUserSendInput unavailable\n");
    return false;
}

static input_mode_t s_input_mode = INPUT_SENDINPUT_VK;

bool keyboard_init(void) { interception_wrapper_init(); return true; }

void keyboard_set_mode(input_mode_t mode)
{
    if (mode >= INPUT_SENDINPUT_VK && mode <= INPUT_DEVICEIO)
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
    case INPUT_SENDINPUT_VK:       return L"SendInput-虚拟键码";
    case INPUT_SENDINPUT_SCANCODE: return L"SendInput-扫描码";
    case INPUT_KEYBD_EVENT:        return L"keybd_event";
    case INPUT_NT_SENDINPUT:       return L"NtUserSendInput";
    case INPUT_INTERCEPTION:       return L"Interception";
    case INPUT_DEVICEIO:           return L"DeviceIoControl";
    default:                       return L"未知";
    }
}

bool keyboard_send(uint16_t key, DWORD flags)
{
    printf("KBD: ENTER\n");fflush(stdout);
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
        UINT scan = MapVirtualKeyW(key, MAPVK_VK_TO_VSC);
        input.ki.wScan = (WORD)(scan & 0x00FF);
        input.ki.dwFlags = flags | KEYEVENTF_SCANCODE;
        if (HIBYTE(scan) == 0xE0)
            input.ki.dwFlags |= KEYEVENTF_EXTENDEDKEY;
        return SendInput(1, &input, sizeof(INPUT)) == 1;
    }
    case INPUT_KEYBD_EVENT:
    {
        BYTE vk = (BYTE)key;
        keybd_event(vk, MapVirtualKeyW(vk, MAPVK_VK_TO_VSC),
                    (flags & KEYEVENTF_KEYUP) ? KEYEVENTF_KEYUP : 0, 0);
        return true;
    }
    case INPUT_NT_SENDINPUT:
    {
        if (!keyboard_load_nt_sendinput())
            return false;
        INPUT input = {0};
        input.type = INPUT_KEYBOARD;
        input.ki.wVk = key;
        input.ki.dwFlags = flags;
        input.ki.time = GetTickCount();
        LONG status = s_NtUserSendInput(1, &input, sizeof(INPUT));
        bool ok = (status >= 0);
        if (!ok)
            printf("Keyboard: NtUserSendInput failed, status=0x%08lX\n", (unsigned long)status);
        return ok;
    }
    case INPUT_INTERCEPTION:
    {
        if (flags & KEYEVENTF_KEYUP)
            return interception_wrapper_key_up(key);
        return interception_wrapper_key_down(key);
    }
    case INPUT_DEVICEIO:
    {
        printf("KBD: DEVICEIO key=0x%04X flags=%lu\n", (unsigned)key, (unsigned long)flags); fflush(stdout);
        if (flags & KEYEVENTF_KEYUP)
            return deviceio_wrapper_key_up(key);
        return deviceio_wrapper_key_down(key);
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
    Sleep(50);
    return keyboard_send(key, KEYEVENTF_KEYUP);
}

void keyboard_deinit(void)
{
    s_NtUserSendInput = NULL;
}
