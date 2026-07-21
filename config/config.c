/**
 * @file config.c
 * @author kilito_hyx (kilito.hyx@gmail.com)
 * @brief 配置持久化管理
 * @version 0.1
 * @date 2026-07-20
 *
 * @copyright Copyright (c) 2026
 *
 */

#include "config.h"
#include "action_list.h"
#include "hotkey.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <windows.h>

#define CONFIG_MAGIC 0x50504B53 // "PPKS"
#define CONFIG_VERSION 1

#define CONFIG_DIRECTORY L"PPKeys"
#define CONFIG_FILE_NAME L"config.dat"

static wchar_t s_config_path[MAX_PATH];

/**
 * @brief 初始化配置模块并加载配置
 *
 * @return true 成功
 * @return false 失败
 */
bool config_build_path(void)
{
    wchar_t app_data_path[MAX_PATH];
    DWORD length = GetEnvironmentVariableW(L"APPDATA", app_data_path, MAX_PATH);
    if (length == 0 || length >= MAX_PATH)
        return false;

    if (swprintf_s(s_config_path, MAX_PATH, L"%ls\\%ls\\%ls", app_data_path, CONFIG_DIRECTORY, CONFIG_FILE_NAME) < 0)
        return false;

    return true;
}

/**
 * @brief 创建配置目录
 *
 * @return true 成功
 * @return false 失败
 */
static bool config_create_directory(void)
{
    wchar_t directory_path[MAX_PATH];
    wchar_t app_data_path[MAX_PATH];

    DWORD length = GetEnvironmentVariableW(L"APPDATA", app_data_path, MAX_PATH);

    if (length == 0 || length >= MAX_PATH)
        return false;

    if (swprintf_s(directory_path, MAX_PATH, L"%ls\\%ls", app_data_path, CONFIG_DIRECTORY) < 0)
        return false;

    if (CreateDirectoryW(directory_path, NULL))
        return true;
    if (GetLastError() == ERROR_ALREADY_EXISTS)
        return true;

    return false;
}

/**
 * @brief 加载配置文件
 *
 * @return true 成功
 * @return false 配置不存在或加载失败
 */
static bool config_load(void)
{
    FILE *file = _wfopen(s_config_path, L"rb");
    if (file == NULL)
        return false;
    config_header_t header;
    if (fread(&header, sizeof(header), 1, file) != 1)
    {
        fclose(file);
        return false;
    }

    if (header.magic != CONFIG_MAGIC || header.version != CONFIG_VERSION)
    {
        fclose(file);
        return false;
    }

    hotkey_set_value(header.hotkey);

    for (uint16_t i = 0; i < header.action_count; i++)
    {
        macro_action_t action;

        if (fread(&action, sizeof(action), 1, file) != 1)
        {
            fclose(file);
            return false;
        }

        if (!action_list_add(&action))
        {
            fclose(file);
            return false;
        }
    }

    fclose(file);

    return true;
}

/**
 * @brief 初始化配置模块
 *
 * @return true 成功
 * @return false 失败
 */
bool config_init(void)
{
    if (!config_build_path())
        return false;
    config_load();
    return true;
}

/**
 * @brief 保存当前配置
 *
 * @return true 成功
 * @return false 失败
 */
bool config_save(void)
{
    if (!config_create_directory())
        return false;

    FILE *file = _wfopen(s_config_path, L"wb");
    if (file == NULL)
        return false;

    config_header_t header = {
        .magic = CONFIG_MAGIC,
        .version = CONFIG_VERSION,
        .hotkey = hotkey_get(),
        .action_count = (uint16_t)action_list_get_count(),
    };
    if (fwrite(&header, sizeof(header), 1, file) != 1)
    {
        fclose(file);
        return false;
    }

    uint32_t action_count = action_list_get_count();

    for (uint32_t i = 0; i < action_count; i++)
    {
        const macro_action_t *action = action_list_get(i);

        if (action == NULL)
        {
            fclose(file);
            return false;
        }
        if (fwrite(action, sizeof(macro_action_t), 1, file) != 1)
        {
            fclose(file);
            return false;
        }
    }
    fclose(file);
    return true;
}

/**
 * @brief 释放配置模块
 *
 */
void config_deinit(void)
{
}