# MsgWorkbench · 消息工作台

MsgWorkbench 是一个用 C++ 和 Qt Widgets 编写的本地消息工作台。它可以手动录入消息，也可以按设定间隔生成测试消息，用来观察列表滚动、正文换行、筛选和历史查询的效果。

## 功能

| 区域 | 用法 |
| --- | --- |
| 消息列表 | 启动时为空。支持按正文筛选，长消息可点「查看全文」，也可以把当前筛选结果保存为 TXT。 |
| 撰写消息 | 填写正文和可选标题，点击「添加消息」，消息会出现在左侧并写入当天的历史记录。 |
| 自动发送消息 | 点击「开始发送」后立即发送第一条，随后按设定间隔持续发送，直到点击「停止发送」。间隔以毫秒为单位，默认 2000 毫秒（2 秒）。正文留空时会使用多行示例。 |
| 历史信息 | 按日期查看、分页浏览和删除历史记录。手动消息和自动消息都保存在这里。 |

左侧列表最多保留 100 条消息。点击「清空」只清空当前列表，历史记录仍可从「历史信息」查询。自动发送期间会保留左侧筛选条件。

## 构建和运行

已在 **Windows、Qt 5.12.8 和 MinGW 64 位**下构建。需要带 C++11 支持的编译器、qmake、Qt Widgets、Qt SQL 和 `QSQLITE` 驱动。

### 使用 Qt Creator

1. 打开 [MsgWorkbench.pro](MsgWorkbench.pro)。
2. 选择 Qt 5.12.8 MinGW 64 位套件。
3. 点击「运行」。

### 使用命令行

将对应 Qt 套件的 `bin` 目录和 MinGW 的 `bin` 目录加入 `PATH`，然后从仓库根目录运行以下命令。

```powershell
mkdir build
cd build
qmake ..\MsgWorkbench.pro -spec win32-g++ "CONFIG+=release"
mingw32-make -j4
.\release\MsgWorkbench.exe
```

如果要把程序复制到没有 Qt 开发环境的 Windows 机器上，可在构建目录中运行 `windeployqt --release .\release\MsgWorkbench.exe`，再复制整个 `release` 目录。

## 数据存放

消息保存在可执行文件同级的 `HisTipMsgs/` 目录中，每天一个 `yyyy-MM-dd.db` 文件。第一次保存消息时会创建该目录，因此程序所在目录需要可写。要备份或迁移历史记录，可以复制整个 `HisTipMsgs/` 目录。

## 项目结构

- [main.cpp](main.cpp) 启动 Qt 应用并加载主题。
- [frameless_window.cpp](frameless_window.cpp) 实现 Windows 自绘标题栏的拖动、缩放和窗口按钮。
- [workbench_title_bar.cpp](workbench_title_bar.cpp) 实现工作台顶部的标题和标识。
- [message_workbench_window.cpp](message_workbench_window.cpp) 组装三栏界面，并连接输入与消息列表。
- [message_composer.cpp](message_composer.cpp) 实现手动输入表单。
- [periodic_message_sender.cpp](periodic_message_sender.cpp) 实现定时发送面板。
- [message_viewer/](message_viewer/) 包含列表、详情、历史对话框和 SQLite 存储，可通过 [message_viewer.pri](message_viewer/message_viewer.pri) 引入其他 qmake 项目。
- [workbench.qss](workbench.qss) 定义工作台主题。

## 第三方代码

消息列表和绘制委托的部分实现源自 Michael Scopchanov 的 [SO-MessageLog](https://github.com/scopchanov/SO-MessageLog) 示例。相关源码保留了原版权声明和 MIT 许可证文本。

应用图标改编自 Bootstrap Icons v1.13.1 的 [chat-left-text-fill](https://icons.getbootstrap.com/icons/chat-left-text-fill/)，调整了配色并增加背景。原图标使用 MIT 许可证，完整声明见 [BOOTSTRAP-ICONS-LICENSE.txt](assets/BOOTSTRAP-ICONS-LICENSE.txt)。可编辑的 SVG 和 Windows ICO 位于 [assets/](assets/)。
