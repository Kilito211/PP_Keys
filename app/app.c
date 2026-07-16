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
#define DEBUG 1

#include "app.h"
#include "app_event.h"
#include "window.h"
#include "hotkey.h"
#include "state_machine.h"
#include "action_list.h"
#include "macro_engine.h"
#include <stdio.h>

#if DEBUG
#include "keyboard.h"
#endif // DEBUG


bool app_init(void)
{
    if (!app_event_init())
        return false;

    if (!action_list_init())
    {
        app_event_deinit();
        return false;
    }

    if (!state_machine_init())
    {
        action_list_deinit();
        app_event_deinit();
        return false;
    }

    if (!hotkey_init())
    {
        state_machine_deinit();
        action_list_deinit();
        app_event_deinit();
        return false;
    }

    if (!window_init())
    {
        hotkey_deinit();
        state_machine_deinit();
        action_list_deinit();
        app_event_deinit();
        return false;
    }

    if (!hotkey_start_listen(window_get_handle()))
    {
        window_deinit();
        hotkey_deinit();
        state_machine_deinit();
        action_list_deinit();
        app_event_deinit();
        return false;
    }

    if (!macro_engine_init())
    {
        hotkey_stop_listen();
        hotkey_deinit();
        window_deinit();
        state_machine_deinit();
        action_list_deinit();
        app_event_deinit();
        return false;
    }

#if DEBUG
    macro_action_t action =
        {
            .key = 0x41,
            .delay_ms = 100,
        };
    macro_action_t action_1 =
        {
            .key = VK_F9,
            .delay_ms = 200,
        };

    action_list_add(&action);
    action_list_add(&action_1);
    uint32_t action_count = action_list_get_count();
    printf("APP: Action count=%lu\n", action_count);
    for (int i = 0; i < action_count; i++)
    {
        macro_action_t *temp = action_list_get(i);
        printf("APP: Action%d -> Key:%d Delay_ms:%d\n", i, temp->key, temp->delay_ms);
    }

    // macro_engine_start();

#endif // DEBUG

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
    macro_engine_deinit();
    hotkey_stop_listen();
    window_deinit();
    hotkey_deinit();
    state_machine_deinit();
    action_list_deinit();
    app_event_deinit();
}