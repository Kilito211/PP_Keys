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

int main(void)
{
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