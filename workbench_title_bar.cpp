#include "workbench_title_bar.h"

#include <QApplication>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>

WorkbenchTitleBar::WorkbenchTitleBar(QWidget *parent)
    : QFrame(parent),
      m_windowControlsLayout(new QHBoxLayout)
{
    setObjectName(QStringLiteral("workbenchBanner"));

    auto *layout = new QHBoxLayout(this);
    layout->setContentsMargins(16, 6, 12, 6);
    layout->setSpacing(12);

    auto *logo = new QLabel(this);
    logo->setPixmap(QApplication::windowIcon().pixmap(28, 28));
    logo->setFixedSize(28, 28);
    logo->setAccessibleName(tr("消息工作台图标"));
    layout->addWidget(logo, 0, Qt::AlignVCenter);

    auto *caption = new QLabel(QStringLiteral("MSG / WORKBENCH"), this);
    caption->setObjectName(QStringLiteral("brandCaption"));
    auto *heading = new QLabel(tr("消息工作台"), this);
    heading->setObjectName(QStringLiteral("workbenchHeading"));
    layout->addWidget(caption, 0, Qt::AlignVCenter);
    layout->addWidget(heading, 0, Qt::AlignVCenter);
    layout->addStretch();

    m_windowControlsLayout->setSpacing(6);
    layout->addLayout(m_windowControlsLayout);
}

QHBoxLayout *WorkbenchTitleBar::windowControlsLayout() const
{
    return m_windowControlsLayout;
}
