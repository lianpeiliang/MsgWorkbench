#include "message_workbench_window.h"
#include "message_composer.h"
#include "message_panel.h"
#include "periodic_message_sender.h"

#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QSplitter>
#include <QVBoxLayout>

MessageWorkbenchWindow::MessageWorkbenchWindow(QWidget *parent)
    : QWidget(parent),
      m_messages(new MessagePanel),
      m_composer(new MessageComposer)
{
    setWindowTitle(tr("消息工作台"));
    setObjectName(QStringLiteral("workbench"));

    auto *workbenchLayout = new QVBoxLayout(this);
    workbenchLayout->setContentsMargins(22, 20, 22, 22);
    workbenchLayout->setSpacing(16);

    auto *banner = new QFrame(this);
    banner->setObjectName(QStringLiteral("workbenchBanner"));
    auto *bannerLayout = new QHBoxLayout(banner);
    bannerLayout->setContentsMargins(20, 16, 20, 16);
    auto *brand = new QVBoxLayout;
    brand->setSpacing(3);
    auto *caption = new QLabel(QStringLiteral("MSG / WORKBENCH"), banner);
    caption->setObjectName(QStringLiteral("brandCaption"));
    auto *heading = new QLabel(tr("消息工作台"), banner);
    heading->setObjectName(QStringLiteral("workbenchHeading"));
    brand->addWidget(caption);
    brand->addWidget(heading);
    bannerLayout->addLayout(brand, 1);
    auto *badge = new QLabel(tr("本地工作区"), banner);
    badge->setObjectName(QStringLiteral("workspaceBadge"));
    bannerLayout->addWidget(badge, 0, Qt::AlignVCenter);
    workbenchLayout->addWidget(banner);

    auto *splitter = new QSplitter(Qt::Horizontal, this);
    splitter->setChildrenCollapsible(false);
    workbenchLayout->addWidget(splitter, 1);

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
