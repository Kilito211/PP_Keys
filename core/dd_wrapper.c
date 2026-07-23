#include <stdio.h>
#include "dd_wrapper.h"

typedef int (*pDD_btn)(int btn);
typedef int (*pDD_key)(int ddcode, int flag);
typedef int (*pDD_todc)(int vkcode);

static HMODULE s_dll = NULL;
static pDD_btn  s_DD_btn  = NULL;
static pDD_key  s_DD_key  = NULL;
static pDD_todc s_DD_todc = NULL;

bool dd_wrapper_init(void)
{
    if (s_dll) return true;

    /* 从 exe 目录加载 DLL */
    wchar_t exePath[MAX_PATH];
    wchar_t dllPath[MAX_PATH];
    DWORD len = GetModuleFileNameW(NULL, exePath, MAX_PATH);
    if (len > 0)
    {
        wchar_t *p = wcsrchr(exePath, L'\\');
        if (p) p[1] = 0;

        swprintf(dllPath, MAX_PATH, L"%lsddhid.63340.dll", exePath);
        s_dll = LoadLibraryW(dllPath);
        if (!s_dll)
        {
            swprintf(dllPath, MAX_PATH, L"%lsdd63330.dll", exePath);
            s_dll = LoadLibraryW(dllPath);
        }
        if (!s_dll)
        {
            swprintf(dllPath, MAX_PATH, L"%lsdd94687.64.dll", exePath);
            s_dll = LoadLibraryW(dllPath);
        }
    }

    if (!s_dll)
    {
        s_dll = LoadLibraryW(L"ddhid.63340.dll");
        if (!s_dll)
            s_dll = LoadLibraryW(L"dd63330.dll");
        if (!s_dll)
            s_dll = LoadLibraryW(L"dd94687.64.dll");
    }

    if (!s_dll)
    {
        printf("DD: dll not found\n"); fflush(stdout);
        return false;
    }

    s_DD_btn  = (pDD_btn)GetProcAddress(s_dll, "DD_btn");
    s_DD_key  = (pDD_key)GetProcAddress(s_dll, "DD_key");
    s_DD_todc = (pDD_todc)GetProcAddress(s_dll, "DD_todc");

    if (!s_DD_btn || !s_DD_key || !s_DD_todc)
    {
        printf("DD: missing exports\n"); fflush(stdout);
        FreeLibrary(s_dll); s_dll = NULL;
        return false;
    }

    int st = s_DD_btn(0);
    if (st != 1)
    {
        printf("DD: init failed (code="); printf("%d", st); printf(")\n"); fflush(stdout);
        FreeLibrary(s_dll); s_dll = NULL;
        return false;
    }

    printf("DD: initialized OK\n"); fflush(stdout);
    return true;
}

bool dd_wrapper_key_down(uint16_t vk)
{
    if (!s_dll && !dd_wrapper_init())
    {
        printf("DD: key_down failed (not initialized)\n"); fflush(stdout);
        return false;
    }
    int ddcode = s_DD_todc((int)vk);
    int ret = s_DD_key(ddcode, DD_KEY_DOWN);
    printf("DD: key_down vk=0x%04X ddcode=%d ret=%d\n", vk, ddcode, ret); fflush(stdout);
    return true;
}

bool dd_wrapper_key_up(uint16_t vk)
{
    if (!s_dll && !dd_wrapper_init())
    {
        printf("DD: key_up failed (not initialized)\n"); fflush(stdout);
        return false;
    }
    int ddcode = s_DD_todc((int)vk);
    int ret = s_DD_key(ddcode, DD_KEY_UP);
    printf("DD: key_up vk=0x%04X ddcode=%d ret=%d\n", vk, ddcode, ret); fflush(stdout);
    return true;
}

void dd_wrapper_deinit(void)
{
    if (s_dll)
    {
        FreeLibrary(s_dll);
        s_dll = NULL;
    }
    s_DD_btn = NULL; s_DD_key = NULL; s_DD_todc = NULL;
    printf("DD: deinitialized\n");
}
