/**
 * @file app_event.c
 * @author kilito_hyx (kilito.hyx@gmail.com)
 * @brief 应用程序事件处理模块
 * @version 0.1
 * @date 2026-07-14
 *
 * @copyright Copyright (c) 2026
 *
 */

#include "app_event.h"
#include <stdio.h>

static void app_event_dispatch(app_event_t event);

/**
 * @brief 初始化应用事件系统
 *
 * V1.0：
 * 不需要创建任何资源。
 *
 * 后续版本：
 * - Event Queue
 * - Worker Thread
 * - Mutex
 * @return true 初始化成功
 * @return false 初始化失败
 */
bool app_event_init(void)
{
    return true;
}

/**
 * @brief 发送事件 直接调用static函数，后续更新为queue时可直接替换
 * 
 * @param event 事件枚举体
 */
void app_event_post(app_event_t event)
{
    app_event_dispatch(event);
}

/**
 * @brief 反初始化应用事件模块
 *
 */
void app_event_deinit(void)
{
}

/**
 * @brief 分发事件处理逻辑
 * 
 * @param event 事件枚举体
 */
static void app_event_dispatch(app_event_t event)
{
    switch (event)
    {
    case APP_EVENT_NONE:
        break;
    case APP_EVENT_START:
        printf("APP_EVENT_START received\n");
        break;
    case APP_EVENT_STOP:
        break;
    case APP_EVENT_EXIT:
        break;

    default:
        break;
    }
}



