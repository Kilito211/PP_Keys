/**
 * @file deviceio_wrapper.h
 * @brief DeviceIoControl -> \\.\\KEYBOARD\\0
 *        直接向 kbdclass 驱动发送 IOCTL 插入扫描码
 *        无需安装第三方驱动,只需管理员权限
 * @version 0.1
 * @date 2026-07-23
 */
#ifndef DEVICEIO_WRAPPER_H
#define DEVICEIO_WRAPPER_H

#include <stdbool.h>
#include <stdint.h>
#include <windows.h>

/*
 * IOCTL_KEYBOARD_INSERT_SCANCODE (0x000BC004)
 * 定义: CTL_CODE(FILE_DEVICE_KEYBOARD, 0x0001, METHOD_BUFFERED, FILE_READ_DATA | FILE_WRITE_DATA)
 * FILE_DEVICE_KEYBOARD = 0x000b
 * 功能: 向键盘类驱动输入队列插入一条 KEYBOARD_INPUT_DATA（硬件扫描码）
 */
#define DEVIO_IOCTL_INSERT_SCANCODE 0x000BC004

/*
 * KEYBOARD_INPUT_DATA 结构
 * Windows WDK: ntddkbd.h
 * 用于 IOCTL_KEYBOARD_INSERT_SCANCODE
 */
typedef struct {
    USHORT UnitId;           /* 键盘单元号(0 = 第一个键盘) */
    USHORT MakeCode;         /* 硬件扫描码(低字节) */
    USHORT Flags;            /* 0x00=按下(MAKE), 0x01=抬起(BREAK), 0x02=E0扩展标志 */
    USHORT Reserved;         /* 保留(0) */
    ULONG  ExtraInformation; /* 附加信息(0) */
} DEVIO_KEYBOARD_INPUT_DATA;

/* Flags */
#define DEVIO_KEY_MAKE  0
#define DEVIO_KEY_BREAK 1
#define DEVIO_KEY_E0    2

bool deviceio_wrapper_init(void);
bool deviceio_wrapper_key_down(uint16_t vk);
bool deviceio_wrapper_key_up(uint16_t vk);
void deviceio_wrapper_deinit(void);

#endif /* DEVICEIO_WRAPPER_H */
