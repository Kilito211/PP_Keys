/**
 * @file main.c
 * @author kilito_hyx (kilito.hyx@gmail.com)
 * @brief 主函数入口
 * @version 0.1
 * @date 2026-07-14
 *
 * @copyright Copyright (c) 2026
 *
 */

#include "app.h"
#include "single_instance.h"
#include "win_util.h"

int main(void)
{
    if (!is_admin())
    {
        if (!run_as_admin())
        {
            MessageBoxW(NULL, L"程序必须以管理员权限运行", L"权限错误", MB_OK | MB_ICONERROR);
            return -1;
        }
        return 0;
    }

    if (!single_instance_check())
    {
        return 0;
    }

    if (!app_init())
    {
        return -1;
    }

    app_run();

    app_deinit();

    return 0;
}