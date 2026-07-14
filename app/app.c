/**
 * @file app.c
 * @author kilito_hyx (kilito.hyx@gmail.com)
 * @brief 应用程序入口
 * @version 0.1
 * @date 2026-07-14
 *
 * @copyright Copyright (c) 2026
 *
 */
#include "app.h"
#include "app_event.h"
#include "window.h"
#include <stdio.h>

bool app_init(void)
{
    if (!app_event_init())
        return false;

    if (!window_init())
    {
        app_event_deinit();
        return false;
    }
    printf("APP Init Success\n");
    return true;
}

void app_run(void)
{
    window_show();

    window_run();
}

void app_deinit(void)
{
    window_deinit();
    app_event_deinit();
}