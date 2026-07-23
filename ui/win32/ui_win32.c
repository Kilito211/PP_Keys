/**
 * @file ui_win32.c
 * @author kilito_hyx (kilito.hyx@gmail.com)
 * @brief Win32除主窗口以外的UI
 * @version 0.1
 * @date 2026-07-16
 *
 * @copyright Copyright (c) 2026
 *
 */

#include "ui_win32.h"
#include "app_event.h"
#include "ui_capture.h"
#include "hotkey.h"
#include "action_list.h"
#include "window.h"
#include "state_machine.h"
#include "config.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#define DEBUG 0
#define ACTION_MAX_UI 6

static HWND s_btn_start = NULL;
static HWND s_btn_hotkey = NULL;
static HWND s_btn_add = NULL;
static HWND s_label_key = NULL;
static HWND s_label_delay = NULL;
static HWND s_label_input_key;
static HWND s_btn_input_key;
static HWND s_label_input_delay;
static HWND s_edit_input_delay;
static HWND s_table_frame;
static uint16_t s_input_action_key = VK_F9;
static ui_action_item_t s_action_items[ACTION_MAX_UI];

static bool ui_get_input_action(macro_action_t *action);
static void ui_reset_input(void);

/**
 * @brief 初始化UI
 *
 * @param parent 父窗口句柄
 * @return true 成功
 * @return false 失败
 */
bool ui_win32_init(HWND parent)
{
    // 创建启停按键
    s_btn_start = CreateWindowExW(0, L"BUTTON", L"Start/Stop", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 150, 350, 110, 32, parent, (HMENU)IDC_BTN_START, GetModuleHandleW(NULL), NULL);
    if (s_btn_start == NULL)
        return false;
    ui_win32_update_state(); // 更新start按键状态

    // 创建热键文本
    if (!CreateWindowExW(0, L"STATIC", L"Hotkey", WS_CHILD | WS_VISIBLE, 20, 24, 60, 20, parent, NULL, GetModuleHandleW(NULL), NULL))
        return false;

    // 创建热键修改按键
    wchar_t text[32];
    hotkey_get_name(hotkey_get(), text, 32);
    s_btn_hotkey = CreateWindowExW(0, L"BUTTON", text, WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 90, 18, 90, 28, parent, (HMENU)IDC_BTN_CHANGE_HOTKEY, GetModuleHandleW(NULL), NULL);
    if (s_btn_hotkey == NULL)
        return false;

    // 创建输入模式选择
    if (!CreateWindowExW(0, L"STATIC", L"Input", WS_CHILD | WS_VISIBLE, 20, 50, 40, 20, parent, NULL, GetModuleHandleW(NULL), NULL))
        return false;
    {
        HWND hCombo = CreateWindowExW(0, L"COMBOBOX", NULL, WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_TABSTOP | WS_VSCROLL, 70, 48, 160, 100, parent, (HMENU)IDC_COMBO_INPUT_MODE, GetModuleHandleW(NULL), NULL);
        if (hCombo == NULL)
            return false;
        SendMessageW(hCombo, CB_ADDSTRING, 0, (LPARAM)keyboard_get_mode_name(INPUT_SENDINPUT_VK));
        SendMessageW(hCombo, CB_ADDSTRING, 0, (LPARAM)keyboard_get_mode_name(INPUT_SENDINPUT_SCANCODE));
        SendMessageW(hCombo, CB_ADDSTRING, 0, (LPARAM)keyboard_get_mode_name(INPUT_KEYBD_EVENT));
        SendMessageW(hCombo, CB_ADDSTRING, 0, (LPARAM)keyboard_get_mode_name(INPUT_NT_SENDINPUT));
        SendMessageW(hCombo, CB_ADDSTRING, 0, (LPARAM)keyboard_get_mode_name(INPUT_INTERCEPTION));
        SendMessageW(hCombo, CB_ADDSTRING, 0, (LPARAM)keyboard_get_mode_name(INPUT_DEVICEIO));
        SendMessageW(hCombo, CB_SETCURSEL, keyboard_get_mode(), 0);
    }

    // 创建Actions标题
    if (!CreateWindowExW(0, L"STATIC", L"Actions", WS_CHILD | WS_VISIBLE, 20, 75, 50, 20, parent, NULL, GetModuleHandleW(NULL), NULL))
        return false;

    // Add按钮
    s_btn_add = CreateWindowExW(0, L"BUTTON", L"Add", WS_CHILD | WS_VISIBLE, 20, 100, 60, 28, parent, (HMENU)IDC_BTN_ADD, GetModuleHandleW(NULL), NULL);
    if (s_btn_add == NULL)
        return false;

    // 添加动作按键的提示文本
    s_label_input_key = CreateWindowExW(0, L"STATIC", L"Key", WS_CHILD | WS_VISIBLE, 95, 105, 40, 20, parent, NULL, GetModuleHandleW(NULL), NULL);
    if (s_label_input_key == NULL)
        return false;

    // 添加动作案件的Edit
    hotkey_get_name(s_input_action_key, text, 32);
    s_btn_input_key = CreateWindowExW(0, L"BUTTON", text, WS_CHILD | WS_VISIBLE | WS_BORDER, 145, 100, 70, 25, parent, (HMENU)IDC_BTN_ACTION_KEY, GetModuleHandleW(NULL), NULL);
    if (s_btn_input_key == NULL)
        return false;

    // 添加delay的提示文本
    s_label_input_delay = CreateWindowExW(0, L"STATIC", L"Delay", WS_CHILD | WS_VISIBLE, 225, 105, 50, 20, parent, NULL, GetModuleHandleW(NULL), NULL);
    if (s_label_input_delay == NULL)
        return false;

    // 添加delay的edit
    s_edit_input_delay = CreateWindowExW(0, L"EDIT", L"100", WS_CHILD | WS_VISIBLE | WS_BORDER, 275, 100, 70, 25, parent, (HMENU)IDC_EDIT_ACTION_DELAY, GetModuleHandleW(NULL), NULL);
    if (s_edit_input_delay == NULL)
        return false;

    // 表格
    s_table_frame = CreateWindowExW(0, L"STATIC", L"", WS_CHILD | WS_VISIBLE | WS_BORDER, 20, 150, 390, 185, parent, NULL, GetModuleHandleW(NULL), NULL);
    if (s_table_frame == NULL)
        return false;

    // 表头
    s_label_key = CreateWindowExW(0, L"STATIC", L"Key", WS_CHILD | WS_VISIBLE, 20, 5, 80, 20, s_table_frame, NULL, GetModuleHandleW(NULL), NULL);
    if (s_label_key == NULL)
        return false;

    s_label_delay = CreateWindowExW(0, L"STATIC", L"Delay(ms)", WS_CHILD | WS_VISIBLE, 120, 5, 100, 20, s_table_frame, NULL, GetModuleHandleW(NULL), NULL);
    if (s_label_delay == NULL)
        return false;

    ui_win32_refresh_action_list();
    return true;
}

/**
 * @brief 按键事件分发
 *
 * @param wParam 低16位为控件ID，高16位为通知码
 * @param lParam 参数2
 * @return true
 * @return false
 */
bool ui_win32_process_command(WPARAM wParam, LPARAM lParam)
{
    uint16_t id = LOWORD(wParam);
    uint16_t notify = HIWORD(wParam);

    if (state_machine_get_state() && id != IDC_BTN_START)
        return true;

    switch (id)
    {
    case IDC_BTN_START:
        app_event_post(APP_EVENT_START);
        return true;
    case IDC_COMBO_INPUT_MODE:
        if (notify == CBN_SELCHANGE)
        {
            HWND hCombo = GetDlgItem(window_get_handle(), IDC_COMBO_INPUT_MODE);
            if (hCombo)
            {
                LRESULT sel = SendMessageW(hCombo, CB_GETCURSEL, 0, 0);
            if (sel >= 0 && sel <= INPUT_DEVICEIO)
                    keyboard_set_mode((input_mode_t)sel);
            }
        }
        return true;
    case IDC_BTN_CHANGE_HOTKEY:
        if (notify == BN_CLICKED)
            ui_capture_begin(UI_CAPTURE_HOTKEY);
        return true;
    case IDC_BTN_ACTION_KEY:
        if (notify == BN_CLICKED)
        {
            ui_capture_begin(UI_CAPTURE_ACTION_KEY);
        }
        return true;
    case IDC_BTN_ADD:
    {
        macro_action_t action;
        if (action_list_get_count() >= ACTION_MAX_UI)
            return true;
        if (!ui_get_input_action(&action))
        {
            printf("Input Invalid\n");
            return true;
        }

        if (!action_list_add(&action))
        {
            printf("Action List Full\n");
            return true;
        }
        config_save();
        ui_win32_refresh_action_list();
        ui_reset_input();
        return true;
    }
    default:
        if (id >= IDC_BTN_DELETE_BASE && id < IDC_BTN_DELETE_BASE + ACTION_MAX_UI)
        {
            uint32_t index = id - IDC_BTN_DELETE_BASE;
            printf("Delete %lu\n", index);
            if (action_list_delete(index))
            {
                config_save();
                ui_win32_refresh_action_list();
            }
            return true;
        }
        return false;
    }
}

/**
 * @brief 获取表格框架窗口句柄
 */
HWND ui_win32_get_table_frame(void)
{
    return s_table_frame;
}


/**
 * @brief 修改热键选项文本
 *
 * @param text
 */
void ui_win32_set_hotkey_text(const wchar_t *text)
{
    if (s_btn_hotkey)
    {
        SetWindowTextW(s_btn_hotkey, text);
    }
}

/**
 * @brief 析构UI
 *
 */
void ui_win32_deinit(void)
{
}

/**
 * @brief 每次添加或删除动作后刷新动作列表
 *
 * @return true 成功
 * @return false 失败
 */
bool ui_win32_refresh_action_list(void)
{
    wchar_t text[64];
    uint32_t count = action_list_get_count(); // 获取动作数量
    if (count > ACTION_MAX_UI)
        count = ACTION_MAX_UI;

    // 清除旧数据
    for (uint32_t i = 0; i < ACTION_MAX_UI; i++)
    {
        if (s_action_items[i].key)
        {
            DestroyWindow(s_action_items[i].key); // 销毁控件
            s_action_items[i].key = NULL;         // 释放指针
        }
        if (s_action_items[i].delay)
        {
            DestroyWindow(s_action_items[i].delay);
            s_action_items[i].delay = NULL;
        }
        if (s_action_items[i].delete_btn)
        {
            DestroyWindow(s_action_items[i].delete_btn);
            s_action_items[i].delete_btn = NULL;
        }
        memset(&s_action_items[i].data, 0, sizeof(macro_action_t));
    }

    // 创建UI行
    for (uint32_t i = 0; i < count; i++)
    {
        const macro_action_t *action = action_list_get(i);

        if (action == NULL)
            continue;

        s_action_items[i].data = *action; // 将数据从执行数据保存到显示静态数据

        int y = 30 + i * UI_ROW_HEIGHT;

        hotkey_get_name(action->key, text, sizeof(text) / sizeof(text[0])); // 获取名字

        s_action_items[i].key = CreateWindowExW(0, L"STATIC", text, WS_CHILD | WS_VISIBLE, 20, y, 80, 20, s_table_frame, NULL, GetModuleHandleW(NULL), NULL); // 创建键值格子
        swprintf(text, 64, L"%lu", action->delay_ms);
        s_action_items[i].delay = CreateWindowExW(0, L"STATIC", text, WS_CHILD | WS_VISIBLE, 120, y, 100, 20, s_table_frame, NULL, GetModuleHandleW(NULL), NULL);                                                      // 创建delay格子
        s_action_items[i].delete_btn = CreateWindowExW(0, L"BUTTON", L"Delete", WS_CHILD | WS_VISIBLE, 300, y - 2 + 150, 70, 22, window_get_handle(), (HMENU)(IDC_BTN_DELETE_BASE + i), GetModuleHandleW(NULL), NULL); // 创建删除按键
        if (!s_action_items[i].key || !s_action_items[i].delay || !s_action_items[i].delete_btn)
        {
            InvalidateRect(s_table_frame, NULL, TRUE);
            InvalidateRect(window_get_handle(), NULL, TRUE);
            return false;
        }
    }
    // 强制刷新表格框架和主窗口，清除 DestroyWindow 留下的残留像素
    InvalidateRect(s_table_frame, NULL, TRUE);
    InvalidateRect(window_get_handle(), NULL, TRUE);
    return true;
}

/**
 * @brief 从两个Edit中读取内容并转化为macro_action_t
 *
 * @param action action数据结构体
 * @return true 成功
 * @return false 失败
 */
static bool ui_get_input_action(macro_action_t *action)
{
    // wchar_t key_text[32];
    wchar_t delay_text[32];

    if (action == NULL)
        return false;

    // GetWindowTextW(s_btn_input_key, key_text, sizeof(key_text) / sizeof(key_text[0]));          // 获取键值文本
    GetWindowTextW(s_edit_input_delay, delay_text, sizeof(delay_text) / sizeof(delay_text[0])); // 获取delay文本

    // if (key_text[0] == L'\0')
    //     return false;

    // SHORT vk = VkKeyScanW(key_text[0]);
    // if (vk == -1)
    //     return false;

    // action->key = LOBYTE(vk);
    action->key = s_input_action_key;
    action->delay_ms = _wtoi(delay_text);
    if (action->delay_ms == 0)
        return false;

    printf("UI_WIN32: key:%d delay:%lu\n", action->key, (unsigned long)action->delay_ms);
    return true;
}

/**
 * @brief Add成功后恢复Exid默认值
 *
 */
static void ui_reset_input(void)
{
    // SetWindowTextW(s_btn_input_key, L"");
    SetWindowTextW(s_edit_input_delay, L"100");

    SetFocus(s_btn_input_key);
}

/**
 * @brief 显示内容
 *
 * @param text
 */
void ui_win32_set_action_key_text(const wchar_t *text)
{
    SetWindowTextW(s_btn_input_key, text);
}

/**
 * @brief 获取键值并显示
 *
 * @param vk 键值
 */
void ui_win32_set_action_key(uint16_t vk)
{
    wchar_t text[32];

    s_input_action_key = vk;

    hotkey_get_name(vk, text, 32);

    ui_win32_set_action_key_text(text);
}

/**
 * @brief 根据APP_STATE更新启停按键文本
 * 
 */
void ui_win32_update_state(void)
{
    if (s_btn_start == NULL)
        return;

    switch (state_machine_get_state())
    {
    case APP_STATE_IDLE:
        SetWindowTextW(s_btn_start, L"Start");
        break;
    case APP_STATE_RUNNING:
        SetWindowTextW(s_btn_start, L"Stop");
        break;
    default:
        break;
    }
}