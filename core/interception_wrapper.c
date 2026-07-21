/**
 * @file interception_wrapper.c
 * @author kilito_hyx (kilito.hyx@gmail.com)
 * @brief Interception 驱动封装实现
 * @version 0.1
 * @date 2026-07-21
 *
 * ---- 前置依赖 ----
 * 用户需自行安装 Interception 驱动和 DLL：
 *   1. 下载 https://github.com/oblitum/Interception/releases
 *   2. 运行 install-interception.exe 安装驱动（需要管理员权限）
 *   3. 将 interception.dll 放到 PPKeys.exe 同级目录
 *
 * ---- 工作原理 ----
 * Interception 驱动注册为 kbdclass 层的键盘过滤驱动。
 * 用户态通过 interception.dll 发送扫描码 -> 驱动在内核态注入
 * -> 输入从 kbdclass 层进入系统，反作弊无法区分与真实硬件的区别。
 *
 * ---- API 说明 ----
 * interception_create_context()  创建上下文
 * interception_set_filter()      设置过滤规则
 * interception_send()            发送 Stroke 到指定设备
 * interception_destroy_context() 销毁上下文
 */
#include "interception_wrapper.h"
#include <stdio.h>

// 函数指针类型定义
typedef void *(*fn_create_context_t)(void);
typedef void  (*fn_destroy_context_t)(void *);
typedef void  (*fn_set_filter_t)(void *, int (*)(interception_device_t), uint16_t);
typedef int   (*fn_send_t)(void *, interception_device_t, const void *, unsigned int);

// 模块和函数指针
static HMODULE s_dll = NULL;
static fn_create_context_t  s_create_context  = NULL;
static fn_destroy_context_t s_destroy_context = NULL;
static fn_set_filter_t      s_set_filter      = NULL;
static fn_send_t            s_send            = NULL;

static void *s_ctx = NULL;           // Interception 上下文
static interception_device_t s_dev = 0; // 目标设备 ID

// 键盘设备谓词
static int is_keyboard(interception_device_t device) {
    return device >= 0 && device < INTCP_MAX_KEYBOARD;
}

bool interception_wrapper_init(void) {
    if (s_ctx)
        return true;

    // 1. 加载 interception.dll
    s_dll = LoadLibraryW(L"interception.dll");
    if (!s_dll) {
        printf("INTCP: interception.dll not found (gle=%lu)\n");
        return false;
    }

    // 2. 获取函数指针
    s_create_context  = (fn_create_context_t)GetProcAddress(s_dll, "interception_create_context");
    s_destroy_context = (fn_destroy_context_t)GetProcAddress(s_dll, "interception_destroy_context");
    s_set_filter      = (fn_set_filter_t)GetProcAddress(s_dll, "interception_set_filter");
    s_send            = (fn_send_t)GetProcAddress(s_dll, "interception_send");

    if (!s_create_context || !s_destroy_context || !s_set_filter || !s_send) {
        printf("INTCP: missing exports\n");
        FreeLibrary(s_dll); s_dll = NULL;
        return false;
    }

    // 3. 创建上下文
    s_ctx = s_create_context();
    if (!s_ctx) {
        printf("INTCP: create_context failed\n");
        FreeLibrary(s_dll); s_dll = NULL;
        return false;
    }

    // 4. 设置键盘过滤
    s_set_filter(s_ctx, is_keyboard, 0x0000);

    // 5. 选择注入设备
    s_dev = INTCP_KEYBOARD(0);

    printf("INTCP: initialized OK\n");
    return true;
}

static bool send_stroke(uint16_t vk, uint16_t state) {
    if (!s_ctx || !s_send)
        return false;

    UINT scan = MapVirtualKeyW(vk, MAPVK_VK_TO_VSC);

    uint8_t stroke_buf[16] = {0};
    interception_key_stroke_t *ks = (interception_key_stroke_t *)stroke_buf;
    ks->code        = (uint16_t)(scan & 0x00FF);
    ks->state       = state;
    ks->information = 0;
    if ((scan >> 8) == 0xE0)
        ks->state |= INTCP_KEY_E0;
    int ret = s_send(s_ctx, s_dev, stroke_buf, 1);
    return (ret > 0);
}

bool interception_wrapper_key_down(uint16_t vk) {
    if (!s_ctx && !interception_wrapper_init()) return false;
    return send_stroke(vk, INTCP_KEY_DOWN);
}
bool interception_wrapper_key_up(uint16_t vk) {
    if (!s_ctx && !interception_wrapper_init()) return false;
    return send_stroke(vk, INTCP_KEY_UP);
}

void interception_wrapper_deinit(void) {
    if (s_ctx && s_destroy_context)
        s_destroy_context(s_ctx);
    s_ctx = NULL;
    if (s_dll) {
        FreeLibrary(s_dll);
        s_dll = NULL;
    }
    printf("INTCP: deinitialized\n");
}