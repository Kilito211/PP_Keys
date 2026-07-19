/**
 * @file macro_engine.c
 * @author kilito_hyx (kilito.hyx@gmail.com)
 * @brief 宏引擎，负责开始、停止、循环执行宏动作
 * @version 0.1
 * @date 2026-07-16
 *
 * @copyright Copyright (c) 2026
 *
 */

#include "macro_engine.h"
#include "action_list.h"
#include "keyboard.h"
#include <stdio.h>
#include <windows.h>

static HANDLE s_thread;
static volatile bool s_running;
static DWORD WINAPI macro_thread(void *arg);

/**
 * @brief 初始化宏引擎
 *
 * @return true 成功
 * @return false 失败
 */
bool macro_engine_init(void)
{
    s_thread = NULL;
    s_running = false;
    return true;
}

/**
 * @brief 启动线程
 *
 * @return true 成功
 * @return false 失败
 */
bool macro_engine_start(void)
{
    if (s_running)
        return false;

    s_running = true;

    // 创建线程，注册线程回调函数
    s_thread = CreateThread(NULL, 0, macro_thread, NULL, 0, NULL);
    if(s_thread == NULL)
    {
        s_running = false;
        return false;
    }

    return true;
}

/**
 * @brief 关闭线程
 * 
 */
void macro_engine_stop(void)
{
    if(!s_running)
        return;

    s_running = false;

    if(s_thread)
    {
        WaitForSingleObject(s_thread, INFINITE); // 等待线程退出
        CloseHandle(s_thread); // 关闭线程
        s_thread = NULL; // 重置指针
    }
}

/**
 * @brief 获取进程运行状态
 * 
 * @return true 正在运行
 * @return false 未在运行
 */
bool macro_engine_is_running(void)
{
    return s_running;
}

/**
 * @brief 析构宏引擎
 * 
 */
void macro_engine_deinit(void)
{
    macro_engine_stop();
}

/**
 * @brief
 *
 * @param arg
 * @return DWORD
 */
static DWORD WINAPI macro_thread(void *arg)
{
    printf("Macro: Thread start\n");

    while (s_running)
    {
        uint32_t count = action_list_get_count();

        for (uint32_t i = 0; i < count && s_running; i++)
        {
            const macro_action_t *action = action_list_get(i);
            if (action == NULL)
                continue;
            keyboard_press(action->key);
            Sleep(action->delay_ms);
        }
    }
    printf("Macro: Thread stop\n");

    return 0;
}