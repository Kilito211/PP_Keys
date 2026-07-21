/**
 * @file interception_wrapper.h
 * @author kilito_hyx (kilito.hyx@gmail.com)
 * @brief Interception 驱动封装 --- 动态加载 interception.dll，绕过用户态反作弊
 * @version 0.1
 * @date 2026-07-21
 *
 * @copyright Copyright (c) 2026
 *
 */

#ifndef INTERCEPTION_WRAPPER_H
#define INTERCEPTION_WRAPPER_H

#include <stdbool.h>
#include <stdint.h>
#include <windows.h>

// .. Interception 数据结构 ..
// .. 参考：https://github.com/oblitum/Interception ..

/// 上下文句柄（不透明指针）
typedef void *interception_context_t;
/// 设备 ID
typedef int interception_device_t;

/// 按键状态标志
#define INTCP_KEY_DOWN  0x00
#define INTCP_KEY_UP    0x01
#define INTCP_KEY_E0    0x02
#define INTCP_KEY_E1    0x04

/// 按键 Stroke 结构体
typedef struct {
    uint16_t code;         // 硬件扫描码
    uint16_t state;        // INTCP_KEY_DOWN / UP，可组合 INTCP_KEY_E0
    uint32_t information;  // 驱动附加信息，填 0
} interception_key_stroke_t;

/// 设备 ID 范围
#define INTCP_MAX_KEYBOARD 10
#define INTCP_KEYBOARD(id) ((interception_device_t)((id) & 0xffff))

// .. 公共 API ..

bool interception_wrapper_init(void);
bool interception_wrapper_key_down(uint16_t vk);
bool interception_wrapper_key_up(uint16_t vk);
void interception_wrapper_deinit(void);

#endif // INTERCEPTION_WRAPPER_H
