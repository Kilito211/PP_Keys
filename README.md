<!-- markdownlint-restore -->

<div align="center">

# PP Keys

✨ **轻量、纯净的 Windows 按键精灵，纯 C 编写，无外部依赖** ✨

<div>
    <img alt="platform" src="https://img.shields.io/badge/platform-Windows-blueviolet">
    <img alt="language" src="https://img.shields.io/badge/language-C11-00599C">
    <img alt="build" src="https://img.shields.io/badge/build-CMake-064F8C">
    <br>
    <img alt="last-commit" src="https://img.shields.io/github/last-commit/Kilito211/PP_Keys">
    <img alt="license" src="https://img.shields.io/github/license/Kilito211/PP_Keys">
</div>
<div>
    <a href="https://github.com/Kilito211/PP_Keys"><img alt="stars" src="https://img.shields.io/github/stars/Kilito211/PP_Keys?style=social"></a>
</div>
<br>

❤ 如果喜欢本项目，右上角送个 `Star` 🌟 吧 ❤

</div>

---

## 📌 目录

- [✨ 功能特性](#-功能特性)
- [🖼️ 界面预览](#️-界面预览)
- [📦 快速构建](#-快速构建)
- [📖 使用说明](#-使用说明)
- [📁 项目结构](#-项目结构)
- [⚙️ 开发相关](#️-开发相关)
- [🔧 待实现](#-待实现)
- [📜 开源许可](#-开源许可)

---

## ✨ 功能特性

### 🎮 核心功能

- [x] **全局热键**
  - 支持 F8 一键启停宏录制
  - 热键可在 UI 中自定义修改
  - 基于 `RegisterHotKey` 实现，低开销

- [x] **动作列表管理**
  - 支持添加/删除按键动作
  - 每个动作可独立设置按键和延迟时间 (ms)
  - 以表格形式清晰展示

- [x] **按键捕获**
  - 点击按钮后按键自动捕获（无需手动填值）
  - 支持热键和动作键两种捕获模式

- [x] **状态悬浮窗**
  - 屏幕右下角半透明悬浮窗，始终置顶
  - 实时显示当前状态（待机中/按键开启/按键关闭）
  - 鼠标穿透，不影响正常操作

- [x] **系统托盘**
  - 最小化到托盘，后台静默运行
  - 托盘菜单支持显示窗口/退出

### 🔊 辅助功能

- [x] **语音播报**
  - 使用 Windows SAPI 语音引擎
  - 启停时自动语音提示"按键开启/关闭"
  - 可与悬浮窗同时使用

### 📐 设计特性

- [x] **纯 C 实现** — 仅依赖 Win32 API + C 标准库，无运行时框架
- [x] **状态机驱动** — 清晰的 IDLE → RUNNING 状态流转
- [x] **模块化架构** — app / core / ui / platform 四层分离
- [x] **双编译器支持** — 同时兼容 MSVC 与 MinGW-w64

---

## 🖼️ 界面预览

> ![1784519495939](C:\Users\Administrator\AppData\Roaming\Typora\typora-user-images\1784519495939.png)

| 主窗口 | 悬浮窗 |
|--------|--------|
| Win32 原生控件，布局简洁 | 右下角半透明，始终置顶 |

---

## 📦 快速构建

### 环境要求

| 编译器 | 构建工具 | 说明 |
|--------|----------|------|
| MSVC 2022+ | CMake ≥ 3.20 | 推荐，完整支持 SAPI |
| MinGW-w64 15+ | CMake ≥ 3.20 | 需系统安装 SAPI 运行时 |

### 构建步骤

```bash
# 克隆仓库
git clone https://github.com/Kilito211/PP_Keys.git
cd PP_Keys

# 配置并构建 (以 MinGW 为例)
cmake -B build -G "MinGW Makefiles"
cmake --build build

# 或使用 MSVC
cmake -B build
cmake --build build

# 运行
./build/PPKeys.exe
```

> **注意**: 语音功能需要 Windows SAPI 运行时支持（Win10/11 自带）。

---

## 📖 使用说明

1. **启动程序** — 主窗口与右下角悬浮窗同时出现，显示"待机中"
2. **添加动作** — 设置按键（点击 Key 按钮后按任意键）→ 设置延迟 → 点击 Add
3. **管理动作** — 列表中的动作支持逐条删除
4. **启停宏** — 按下全局热键 `F8`，状态切换为"按键开启"，再次按下停止
5. **托盘操作** — 关闭窗口最小化到托盘，右键托盘图标可显示窗口或退出

---

## 📁 项目结构

```
PP_Keys/
├── app/                  # 应用层
│   ├── app.c             # 应用初始化、模块组装
│   ├── app_event.c       # 事件系统
│   └── include/          # 应用层头文件
├── core/                 # 核心逻辑
│   ├── state_machine.c   # 状态机 (IDLE / RUNNING)
│   ├── macro_engine.c    # 宏执行引擎（线程循环）
│   ├── hotkey.c          # 热键注册与管理
│   ├── keyboard.c        # 键盘模拟 (SendInput)
│   ├── action_list.c     # 动作列表管理
│   └── single_instance.c # 单实例检查
├── platform/             # 平台层
│   └── win32/
│       ├── window.c      # 主窗口
│       ├── message_loop.c# 消息循环分发
│       ├── tray.c        # 系统托盘
│       ├── voice.c       # SAPI 语音播报
│       ├── status_overlay.c  # 状态悬浮窗
│       └── win_util.c    # Win32 工具函数
├── ui/                   # UI 层
│   ├── ui.c / ui_capture.c   # UI 初始化与按键捕获
│   └── win32/ui_win32.c      # Win32 控件界面
├── config/               # 配置模块（待实现）
├── include/              # 公共头文件（待扩展）
├── resource/             # 资源文件（图标等）
├── CMakeLists.txt        # 顶层构建脚本
└── main.c                # 程序入口
```

---

## ⚙️ 开发相关

### 编码规范

- 语言标准: **C11** (`-std=c11`)
- 编码: **UTF-8**（源文件无 BOM）
- 换行符: **LF**
- 命名风格: `snake_case`（函数、变量），`UPPER_CASE`（宏、枚举）
- 注释: 中文 Doxygen 风格

### 构建配置

CMakeLists.txt 已预设 `/utf-8` 编译选项（MSVC），确保中文注释正常显示。

### 模块依赖关系

```
main.c
  └─ app
       ├─ app_event
       ├─ action_list
       ├─ state_machine
       │    ├─ macro_engine
       │    ├─ voice         (platform)
       │    ├─ status_overlay (platform)
       │    └─ ui_win32      (ui)
       ├─ hotkey
       ├─ window (platform)
       └─ tray   (platform)
```

---

## 🔧 待实现

- [ ] **配置持久化** — 保存/加载动作列表、热键设置（INI / JSON）
- [ ] **UI 美化** — 自定义字体、颜色、控件样式
- [ ] **功能开关** — 语音播报开关、悬浮窗显隐、开机自启
- [ ] **多配置支持** — 多个动作配置的保存与切换
- [ ] **鼠标动作** — 支持鼠标点击、移动等操作
- [ ] **循环模式** — 支持设置循环次数或无限循环
- [ ] **快捷键暂停/恢复** — 在执行中临时暂停

---

## 📜 开源许可

<div align="center">

[![License: GPL v3](https://img.shields.io/badge/License-GPL%20v3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)

PP_Keys 使用 [GNU General Public License v3.0](LICENSE) 开源许可证

Copyright © 2026 Kilito (kilito.hyx@gmail.com)

</div>