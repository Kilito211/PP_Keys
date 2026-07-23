#ifndef DD_WRAPPER_H
#define DD_WRAPPER_H

#include <stdbool.h>
#include <stdint.h>
#include <windows.h>

/* DD_btn(0) 返回值 */
#define DD_INIT_OK          1   // 初始化成功
#define DD_INIT_NO_ADMIN    2   // 需要管理员权限
#define DD_INIT_NO_DRIVER   3   // 驱动未安装
#define DD_INIT_VER_MISMATCH 4  // 版本不匹配

/* DD_key flag */
#define DD_KEY_DOWN 1  // 按下
#define DD_KEY_UP   2  // 抬起

bool dd_wrapper_init(void);
bool dd_wrapper_key_down(uint16_t vk);
bool dd_wrapper_key_up(uint16_t vk);
void dd_wrapper_deinit(void);

#endif
