/**
 * @file state_machine.c
 * @author kilito_hyx (kilito.hyx@gmail.com)
 * @brief 全局状态机
 * @version 0.1
 * @date 2026-07-15
 *
 * @copyright Copyright (c) 2026
 *
 */

#include "state_machine.h"
#include <stdio.h>

static app_state_t s_current_state = APP_STATE_IDLE; // 静态状态值，初始化为空闲

/**
 * @brief 初始化状态机
 *
 * @return true 初始化成功
 * @return false 初始化失败
 */
bool state_machine_init(void)
{
    s_current_state = APP_STATE_IDLE;
    printf("State machine init\n");
    return true;
}

/**
 * @brief 处理应用事件
 *
 * @param event 应用事件
 */
void state_machine_handle_event(app_event_t event)
{
    switch (s_current_state)
    {
    case APP_STATE_IDLE: // 当前为空闲
        switch (event)
        {
        case APP_EVENT_START: // 开始事件
            s_current_state = APP_STATE_RUNNING;
            printf("State: IDLE -> RUNNING\n");
            break;
        default:
            break;
        }
        break;
    case APP_STATE_RUNNING: // 当前为运行中
        switch (event)
        {
        case APP_EVENT_START:
            s_current_state = APP_STATE_IDLE;
            printf("State: RUNNING -> IDLE\n");
            break;

        default:
            break;
        }
        break;
    default:
        s_current_state = APP_STATE_IDLE;
        break;
    }
}

/**
 * @brief 获取当前状态
 */
app_state_t state_machine_get_state(void)
{
    return s_current_state;
}

/**
 * @brief 状态机释放
 */
void state_machine_deinit(void)
{
}