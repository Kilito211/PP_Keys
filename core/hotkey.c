/**
 * @file hotkey.c
 * @author kilito_hyx (kilito.hyx@gmail.com)
 * @brief 注销、修改热键
 * @version 0.1
 * @date 2026-07-14
 *
 * @copyright Copyright (c) 2026
 *
 */

#include "hotkey.h"

/**
 * @brief 初始化热键模块
 * 
 * @return true 初始化成功
 * @return false 初始化失败
 */
bool hotkey_init(void)
{
    return true;
}

/**
 * @brief 注册热键
 * 
 * @param id 热键ID
 * @param vk Win32 Virtual Key
 * @return true 注册成功
 * @return false 注册失败
 */
bool hotkey_register(hotkey_id_t id, unsigned int vk)
{
    (void)id;
    (void)vk;

    return false;
}

/**
 * @brief 注销热键
 * 
 * @param id 热键id
 */
void hotkey_unregister(hotkey_id_t id)
{
    (void)id;
}

/**
 * @brief 处理热键事件
 * 
 * @param id 热键ID
 * @return true 已处理
 * @return false 未处理
 */
bool hotkey_process(unsigned int id)
{
    (void)id;

    return false;
}

/**
 * @brief 反初始化热键模块
 * 
 */
void hotkey_deinit(void)
{
}