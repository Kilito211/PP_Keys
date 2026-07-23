/**
 * @file interception_wrapper.c
 * @author kilito_hyx (kilito.hyx@gmail.com)
 * @brief 自定义Interception键盘驱动
 * @version 0.2
 * @date 2026-07-23
 *
 * ---- Pre-requisites ----
 * 1. Download Interception from https://github.com/oblitum/Interception/releases
 * 2. Run install-interception.exe (admin required) to install driver
 * 3. Place interception.dll next to PPKeys.exe
 *
 * ---- How it works ----
 * Interception registers as a kbdclass-layer keyboard filter driver.
 * This file uses a relay-thread architecture:
 *   1. Intercept ALL keyboard devices (filter=0xFFFF)
 *   2. A relay thread loops: interception_wait -> receive -> send
 *      forwarding physical keystrokes back so keyboard keeps working
 *   3. When injecting, call interception_send() with synthetic strokes.
 *      Driver injects at kernel level, anti-cheat cannot distinguish.
 *
 * ---- Function pointers (loaded dynamically from interception.dll) ----
 * interception_create_context()  - create a driver context handle
 * interception_set_filter()      - set interception filter rules
 * interception_wait()            - block until device event
 * interception_receive()         - receive stroke from device
 * interception_send()            - send stroke to device
 * interception_destroy_context() - destroy driver context
 */
#include "interception_wrapper.h"
#include <stdio.h>
#include <process.h>

/* function pointer typedefs */
typedef void *(*fn_create_context_t)(void);
typedef void  (*fn_destroy_context_t)(void *);
typedef void  (*fn_set_filter_t)(void *, int (*)(interception_device_t), uint16_t);
typedef int   (*fn_wait_t)(void *);
typedef int   (*fn_receive_t)(void *, interception_device_t, void *, unsigned int);
typedef int   (*fn_send_t)(void *, interception_device_t, const void *, unsigned int);

/* module handles and function pointers */
static HMODULE s_dll = NULL;
static fn_create_context_t  s_create_context  = NULL;
static fn_destroy_context_t s_destroy_context = NULL;
static fn_set_filter_t      s_set_filter      = NULL;
static fn_wait_t            s_wait            = NULL;
static fn_receive_t         s_receive         = NULL;
static fn_send_t            s_send            = NULL;
static void *s_ctx = NULL;              /* Interception context handle */
static HANDLE s_relay_thread = NULL;    /* relay thread handle */
static volatile bool s_relay_running = false; /* relay thread control flag */

/* predicate: check if device ID is a keyboard */
static int is_keyboard(interception_device_t device)
{
    return device >= 0 && device < INTCP_MAX_KEYBOARD;
}

/*
 * Relay thread function
 * Loops: wait -> receive -> send (forward to same device)
 * Ensures physical keyboard keeps working despite intercepting all
 */
static unsigned int __stdcall relay_thread_fn(void *arg)
{
    (void)arg;
    while (s_relay_running && s_ctx && s_wait && s_receive && s_send)
    {
        /* block until a device generates an event */
        interception_device_t device = s_wait(s_ctx);
        if (device == 0)
        {
            Sleep(10);
            continue;
        }
        /* receive the stroke data from the device */
        unsigned char stroke_buf[sizeof(interception_key_stroke_t) + 8];
        ZeroMemory(stroke_buf, sizeof(stroke_buf));
        int received = s_receive(s_ctx, device, stroke_buf, 1);
        if (received <= 0) continue;
        /* forward the stroke back to the same device */
        s_send(s_ctx, device, stroke_buf, 1);
    }
    return 0;
}

/* Start the relay thread (also sets interception filter to 0xFFFF) */
static bool start_relay_thread(void)
{
    if (s_relay_thread) return true;
    s_relay_running = true;
    s_set_filter(s_ctx, is_keyboard, 0xFFFF);
    unsigned thread_id = 0;
    s_relay_thread = (HANDLE)_beginthreadex(NULL, 0, relay_thread_fn, NULL, 0, &thread_id);
    if (!s_relay_thread) { s_relay_running = false; return false; }
    return true;
}

/* Stop the relay thread */
static void stop_relay_thread(void)
{
    if (!s_relay_thread) return;
    s_relay_running = false;
    WaitForSingleObject(s_relay_thread, 2000);
    CloseHandle(s_relay_thread);
    s_relay_thread = NULL;
}

/* Public API: initialize Interception context and start relay thread */
bool interception_wrapper_init(void)
{
    if (s_ctx) return true;
    /* Step 1: Load interception.dll at runtime */
    s_dll = LoadLibraryW(L"interception.dll");
    if (!s_dll)
    {
        printf("INTCP: interception.dll not found (gle=");
        printf("%lu", GetLastError());
        printf(")\n");
        return false;
    }
    /* Step 2: Resolve all function pointers from the DLL */
    s_create_context  = (fn_create_context_t)GetProcAddress(s_dll, "interception_create_context");
    s_destroy_context = (fn_destroy_context_t)GetProcAddress(s_dll, "interception_destroy_context");
    s_set_filter      = (fn_set_filter_t)GetProcAddress(s_dll, "interception_set_filter");
    s_wait            = (fn_wait_t)GetProcAddress(s_dll, "interception_wait");
    s_receive         = (fn_receive_t)GetProcAddress(s_dll, "interception_receive");
    s_send            = (fn_send_t)GetProcAddress(s_dll, "interception_send");
    if (!s_create_context || !s_destroy_context || !s_set_filter || !s_wait || !s_receive || !s_send)
    { printf("INTCP: missing exports from interception.dll\n"); FreeLibrary(s_dll); s_dll = NULL; return false; }
    /* Step 3: Create Interception driver context */
    s_ctx = s_create_context();
    if (!s_ctx) { printf("INTCP: interception_create_context failed\n"); FreeLibrary(s_dll); s_dll = NULL; return false; }
    /* Step 4: Start relay thread */
    if (!start_relay_thread())
    { printf("INTCP: start_relay_thread failed\n"); s_destroy_context(s_ctx); s_ctx = NULL; FreeLibrary(s_dll); s_dll = NULL; return false; }
    printf("INTCP: initialized OK\n");
    return true;
}

/*
 * Internal helper: send a single key stroke
 * @param vk    - Windows virtual key code (VK_*)
 * @param state - INTCP_KEY_DOWN (press) or INTCP_KEY_UP (release)
 * @return true on success
 */
static bool send_stroke(uint16_t vk, uint16_t state)
{
    if (!s_ctx || !s_send) return false;
    /* Convert virtual key to hardware scan code */
    UINT scan = MapVirtualKeyW(vk, MAPVK_VK_TO_VSC);
    /* Build the Interception key stroke structure */
    interception_key_stroke_t ks;
    ZeroMemory(&ks, sizeof(ks));
    ks.code        = (uint16_t)(scan & 0x00FF);  /* scancode low byte */
    ks.state       = state;                       /* down/up flag */
    ks.information = 0;                           /* reserved */
    /* Extended keys (arrows, Ins, Del, Home, End) have 0xE0 prefix */
    if ((scan >> 8) == 0xE0)
        ks.state |= INTCP_KEY_E0;
    /* Send stroke to keyboard device 0 */
    int ret = s_send(s_ctx, INTCP_KEYBOARD(0), &ks, 1);
    return (ret > 0);
}

/* Public API: press a key */
bool interception_wrapper_key_down(uint16_t vk)
{
    if (!s_ctx && !interception_wrapper_init()) return false;
    return send_stroke(vk, INTCP_KEY_DOWN);
}

/* Public API: release a key */
bool interception_wrapper_key_up(uint16_t vk)
{
    if (!s_ctx && !interception_wrapper_init()) return false;
    return send_stroke(vk, INTCP_KEY_UP);
}

/* Public API: cleanup */
void interception_wrapper_deinit(void)
{
    stop_relay_thread();
    if (s_ctx && s_destroy_context) { s_destroy_context(s_ctx); s_ctx = NULL; }
    if (s_dll) { FreeLibrary(s_dll); s_dll = NULL; }
    s_create_context = NULL; s_destroy_context = NULL; s_set_filter = NULL;
    s_wait = NULL; s_receive = NULL; s_send = NULL;
}
