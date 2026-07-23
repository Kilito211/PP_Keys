#include <stdio.h>
#include "deviceio_wrapper.h"

static HANDLE s_hKbd = INVALID_HANDLE_VALUE;

bool deviceio_wrapper_init(void)
{
    if (s_hKbd != INVALID_HANDLE_VALUE)
        return true;
    wchar_t path[32];
    for (int i = 0; i < 8; i++)
    {
        swprintf(path, 32, L"\\\\.\\KEYBOARD\\%d", i);
        s_hKbd = CreateFileW(path,
            GENERIC_READ | GENERIC_WRITE,
            FILE_SHARE_READ | FILE_SHARE_WRITE,
            NULL, OPEN_EXISTING, 0, NULL);
        if (s_hKbd != INVALID_HANDLE_VALUE)
        {
            printf("DEVIO: opened %ls\n", path);
            return true;
        }
    }
    printf("DEVIO: no keyboard device, returning fallback OK\n");
    return true;
}

static bool send_stroke(uint16_t vk, uint16_t flags)
{
    if (s_hKbd == INVALID_HANDLE_VALUE && !deviceio_wrapper_init())
        return false;
    if (s_hKbd == INVALID_HANDLE_VALUE)
        return false;

    UINT scan = MapVirtualKeyW(vk, MAPVK_VK_TO_VSC);
    DEVIO_KEYBOARD_INPUT_DATA kid;
    ZeroMemory(&kid, sizeof(kid));
    kid.UnitId = 0;
    kid.MakeCode = (USHORT)(scan & 0x00FF);
    kid.Flags = flags;
    kid.Reserved = 0;
    kid.ExtraInformation = 0;
    if ((scan >> 8) == 0xE0)
        kid.Flags |= DEVIO_KEY_E0;

    DWORD bytesReturned = 0;
    BOOL ok = DeviceIoControl(s_hKbd, DEVIO_IOCTL_INSERT_SCANCODE,
        &kid, sizeof(kid), NULL, 0, &bytesReturned, NULL);
    if (!ok)
    {
        DWORD err = GetLastError();
        printf("DEVIO: IOCTL failed (err=%lu)\n", err);
    }
    return ok ? true : false;
}

bool deviceio_wrapper_key_down(uint16_t vk) { return send_stroke(vk, DEVIO_KEY_MAKE); }
bool deviceio_wrapper_key_up(uint16_t vk)   { return send_stroke(vk, DEVIO_KEY_BREAK); }

void deviceio_wrapper_deinit(void)
{
    if (s_hKbd != INVALID_HANDLE_VALUE)
    {
        CloseHandle(s_hKbd);
        s_hKbd = INVALID_HANDLE_VALUE;
    }
}
