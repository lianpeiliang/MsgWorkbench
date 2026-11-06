#include "workbench_title_bar.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>

WorkbenchTitleBar::WorkbenchTitleBar(QWidget *parent)
    : QFrame(parent),
      m_windowControlsLayout(new QHBoxLayout)
{
    setObjectName(QStringLiteral("workbenchBanner"));

    auto *layout = new QHBoxLayout(this);
    layout->setContentsMargins(20, 16, 20, 16);

    auto *brand = new QVBoxLayout;
    brand->setSpacing(3);
    auto *caption = new QLabel(QStringLiteral("MSG / WORKBENCH"), this);
    caption->setObjectName(QStringLiteral("brandCaption"));
    auto *heading = new QLabel(tr("消息工作台"), this);
    heading->setObjectName(QStringLiteral("workbenchHeading"));
    brand->addWidget(caption);
    brand->addWidget(heading);
    layout->addLayout(brand, 1);

    auto *badge = new QLabel(tr("本地工作区"), this);
    badge->setObjectName(QStringLiteral("workspaceBadge"));
    layout->addWidget(badge, 0, Qt::AlignVCenter);
    layout->addLayout(m_windowControlsLayout);
}

QHBoxLayout *WorkbenchTitleBar::windowControlsLayout() const
{
    return m_windowControlsLayout;
}
