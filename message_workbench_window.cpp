#include "message_workbench_window.h"
#include "message_composer.h"
#include "message_panel.h"
#include "periodic_message_sender.h"
#include "workbench_title_bar.h"

#include <QMessageBox>
#include <QSplitter>
#include <QVBoxLayout>

MessageWorkbenchWindow::MessageWorkbenchWindow(QWidget *parent)
    : FramelessWindow(parent),
      m_messages(new MessagePanel),
      m_composer(new MessageComposer)
{
    setWindowTitle(tr("消息工作台"));
    setObjectName(QStringLiteral("workbench"));

    auto *workbenchLayout = new QVBoxLayout(this);
    workbenchLayout->setContentsMargins(0, 0, 0, 0);
    workbenchLayout->setSpacing(0);

    auto *titleBar = new WorkbenchTitleBar(this);
    attachTitleBar(titleBar, titleBar->windowControlsLayout());
    workbenchLayout->addWidget(titleBar);

    // 只有三栏内容需要留白，标题栏直接贴住窗口边缘。
    auto *contentLayout = new QVBoxLayout;
    contentLayout->setContentsMargins(22, 16, 22, 22);
    workbenchLayout->addLayout(contentLayout, 1);

    auto *splitter = new QSplitter(Qt::Horizontal, this);
    splitter->setChildrenCollapsible(false);
    contentLayout->addWidget(splitter);

    splitter->addWidget(m_messages);
    splitter->addWidget(m_composer);
    auto *autoSender = new PeriodicMessageSender;
    splitter->addWidget(autoSender);
    connect(m_composer, &MessageComposer::submitRequested, this, &MessageWorkbenchWindow::submitMessage);
    connect(autoSender, &PeriodicMessageSender::sendRequested, this,
            [this, autoSender](const QString &title, const QString &body) {
        const bool saved = m_messages->addMessage(title, body, MessagePanel::FilterPolicy::KeepCurrent);
        autoSender->reportResult(saved);
    });
    splitter->setStretchFactor(0, 3);
    splitter->setStretchFactor(1, 1);
    splitter->setStretchFactor(2, 1);
    resize(1280, 660);
    splitter->setSizes({650, 310, 250});
}

void MessageWorkbenchWindow::submitMessage(const QString &title, const QString &body)
{
    // 保存失败时保留表单内容，用户可以直接重试。
    if (!m_messages->addMessage(title, body))
    {
        QMessageBox::warning(this, tr("添加消息"), tr("无法保存消息，请检查程序目录的写入权限。"));
        return;
    }

    m_composer->reset();
}
