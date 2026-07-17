/**
 * @file window.c
 * @author kilito_hyx (kilito.hyx@gmail.com)
 * @brief Win32窗口实现
 * @version 0.1
 * @date 2026-07-14
 *
 * @copyright Copyright (c) 2026
 *
 */

#include "window.h"
#include "ui_win32.h"
#include "message_loop.h"
#include "tray.h"
#include <windows.h>

#define IDI_ICON1 101
#define WM_SHOW_WINDOW (WM_USER + 10)

static HINSTANCE s_hInstance = NULL; // 全局实例句柄
static HWND s_hWnd = NULL;           // 全局窗口句柄
static bool s_hide_to_tray = true;   // 是否隐藏了窗口

static const wchar_t *s_window_class_name = L"PPKeysWindowClass";  // 窗口类名
static const wchar_t *s_window_title = L"PPKeys";                  // 窗口标题
static const wchar_t *s_show_text = L"Hello Win32 窗口文字测试！"; // 测试文本

static LRESULT CALLBACK window_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

/**
 * @brief 初始化窗口系统
 *         注册窗口类，创建窗口等初始化工作
 *
 * @return true 初始化成功
 * @return false 初始化失败
 */
bool window_init(void)
{
    s_hInstance = GetModuleHandleW(NULL); // 获取当前程序句柄

    WNDCLASSEXW window_class = {
        .cbSize = sizeof(WNDCLASSEXW),        // 结构体大小
        .lpfnWndProc = window_proc,           // 窗口事件处理函数句柄
        .hInstance = s_hInstance,             // 当前程序句柄
        .lpszClassName = s_window_class_name, // 窗口类名
        .hIcon = LoadIconW(s_hInstance, MAKEINTRESOURCEW(IDI_ICON1)),
        .hIconSm = LoadIconW(s_hInstance, MAKEINTRESOURCEW(IDI_ICON1)),
    };

    if (!RegisterClassExW(&window_class)) // 注册窗口类
    {
        return false; // 注册失败
    }

    // 创建窗口
    s_hWnd = CreateWindowExW(0, s_window_class_name, s_window_title, WS_OVERLAPPEDWINDOW & ~(WS_MAXIMIZEBOX | WS_THICKFRAME), CW_USEDEFAULT, CW_USEDEFAULT, 430, 420, NULL, NULL, s_hInstance, NULL);
    if (!s_hWnd) // 创建窗口失败
        return false;

    if (!ui_win32_init(s_hWnd))
    {
        DestroyWindow(s_hWnd);
        return false;
    }

    return true;
}

/**
 * @brief 显示主窗口
 *
 */
void window_show(void)
{
    ShowWindow(s_hWnd, SW_SHOW); // 显示窗口
    UpdateWindow(s_hWnd);        // 刷新窗口
}

/**
 * @brief 进入Windows 消息循环
 *
 */
void window_run(void)
{
    MSG msg;
    while (GetMessageW(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg); // 翻译消息
        DispatchMessageW(&msg); // 分发消息
    }
}

/**
 * @brief 销毁窗口资源
 *
 */
void window_deinit(void)
{
    ui_win32_deinit();
    tray_remove();
}

/**
 * @brief 窗口消息处理函数
 *
 * @param hwnd 窗口句柄
 * @param msg 事件类型
 * @param wParam 消息参数1
 * @param lParam 消息参数2
 * @return LRESULT
 */
static LRESULT CALLBACK window_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    // 先处理消息循环分发器，如果返回true表示消息已处理，直接返回0
    if (message_loop_dispatch(hwnd, msg, wParam, lParam))
    {
        return 0;
    }

    switch (msg)
    {
    case WM_CLOSE:
        if (s_hide_to_tray)
        {
            ShowWindow(
                hwnd,
                SW_HIDE);

            return 0;
        }
        DestroyWindow(hwnd);
        return 0;
    case WM_DESTROY:        // 窗口销毁事件
        tray_remove();      // 销毁托盘
        PostQuitMessage(0); // 发送退出消息
        return 0;
    case WM_SHOW_WINDOW:
        ShowWindow(hwnd, SW_SHOW);
        SetForegroundWindow(hwnd);
        return 0;
#if DEBUG
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        // 获取绘图设备上下文HDC
        HDC hdc = BeginPaint(hwnd, &ps);

        // 方式1：简单定点输出文字 (x=50,y=50)
        TextOutW(hdc, 50, 50, s_show_text, lstrlenW(s_show_text));

        // 可选：设置字体颜色、背景透明
        SetTextColor(hdc, RGB(0, 128, 255)); // 蓝色文字
        SetBkMode(hdc, TRANSPARENT);         // 文字背景透明

        // 再输出一行文字
        const wchar_t *line2 = L"第二行测试文字，位置(50,100)";
        TextOutW(hdc, 50, 100, line2, lstrlenW(line2));

        // 结束绘图，释放HDC
        EndPaint(hwnd, &ps);
        return 0;
    }
#endif
    case WM_COMMAND:
        if (ui_win32_process_command(wParam, lParam))
            return 0;
        break;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam); // 调用Win32默认事件处理
}

/**
 * @brief 传递窗口句柄
 *
 * @return HWND 窗口句柄
 */
HWND window_get_handle(void)
{
    return s_hWnd;
}