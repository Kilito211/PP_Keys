/**
 * @file action_list.c
 * @author kilito_hyx (kilito.hyx@gmail.com)
 * @brief 宏动作列表管理
 * @version 0.1
 * @date 2026-07-15
 *
 * @copyright Copyright (c) 2026
 *
 */

#include "action_list.h"

static macro_action_t s_actions[ACTION_LIST_MAX_SIZE]; // 60个按键的结构体数组
static uint32_t s_action_count = 0;                    // 按键数量计数

/**
 * @brief 初始化动作列表
 *
 * @return true 成功
 * @return false 失败
 */
bool action_list_init(void)
{
    s_action_count = 0;
    return 0;
}

/**
 * @brief 添加宏动作
 *
 * @param action 宏动作数据
 * @return true 成功
 * @return false 失败
 */
bool action_list_add(const macro_action_t *action)
{
    if (action == NULL)
        return false;
    if (s_action_count >= ACTION_LIST_MAX_SIZE)
        return false;

    s_actions[s_action_count] = *action;
    s_action_count++;
    return true;
}

/**
 * @brief 获取动作数量
 *
 * @return uint32_t 数量
 */
uint32_t action_list_get_count(void)
{
    return s_action_count;
}

/**
 * @brief 获取指定动作
 *
 * @param index 索引
 * @return const macro_action_t* 动作指针
 */
const macro_action_t *action_list_get(uint32_t index)
{
    if (index >= s_action_count)
        return NULL;

    return &s_actions[index];
}

/**
 * @brief 清空动作列表
 *
 */
void action_list_clear(void)
{
    s_action_count = 0;
}

/**
 * @brief 释放动作列表
 *
 */
void action_list_deinit(void)
{
    
}