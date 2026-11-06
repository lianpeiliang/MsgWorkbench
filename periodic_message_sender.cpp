#include "periodic_message_sender.h"

#include <QLabel>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QVBoxLayout>

PeriodicMessageSender::PeriodicMessageSender(QWidget *parent)
    : QFrame(parent),
      m_interval(new QSpinBox(this)),
      m_body(new QPlainTextEdit(this)),
      m_status(new QLabel(tr("未运行"), this)),
      m_count(new QLabel(this)),
      m_toggleButton(new QPushButton(tr("开始发送"), this))
{
    setObjectName(QStringLiteral("autoSenderPanel"));
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(18, 16, 18, 16);
    layout->setSpacing(10);

    auto *heading = new QLabel(tr("自动发送消息"), this);
    heading->setProperty("sectionHeading", true);
    layout->addWidget(heading);

    auto *intervalLabel = new QLabel(tr("发送间隔"), this);
    intervalLabel->setBuddy(m_interval);
    m_interval->setRange(1, 3600000);
    m_interval->setValue(2000);
    m_interval->setSuffix(tr(" 毫秒"));
    layout->addWidget(intervalLabel);
    layout->addWidget(m_interval);

    auto *bodyLabel = new QLabel(tr("测试正文"), this);
    bodyLabel->setBuddy(m_body);
    m_body->setPlaceholderText(tr("留空时发送默认的多行示例"));
    layout->addWidget(bodyLabel);
    layout->addWidget(m_body, 1);

    m_status->setObjectName(QStringLiteral("autoSendStatus"));
    m_status->setWordWrap(true);
    layout->addWidget(m_status);
    layout->addWidget(m_count);
    updateCount();

    m_toggleButton->setCursor(Qt::PointingHandCursor);
    layout->addWidget(m_toggleButton, 0, Qt::AlignRight);
    connect(m_toggleButton, &QPushButton::clicked, this, &PeriodicMessageSender::toggleSending);
    m_timer.setTimerType(Qt::PreciseTimer);
    connect(&m_timer, &QTimer::timeout, this, &PeriodicMessageSender::sendNext);
}

void PeriodicMessageSender::reportResult(bool saved)
{
    if (!saved)
    {
        stopSending(tr("保存失败，自动发送已停止。"));
        return;
    }

    ++m_sentCount;
    updateCount();
}

void PeriodicMessageSender::toggleSending()
{
    if (m_timer.isActive())
    {
        stopSending(tr("已停止"));
        return;
    }

    m_sentCount = 0;
    updateCount();
    m_interval->setEnabled(false);
    m_status->setText(tr("正在发送"));
    m_toggleButton->setText(tr("停止发送"));
    m_timer.start(m_interval->value());
    sendNext();
}

void PeriodicMessageSender::sendNext()
{
    QString body = m_body->toPlainText();
    if (body.trimmed().isEmpty())
    {
        body = tr("这是一条自动生成的测试消息。\n第二行用于查看换行效果。");
    }
    emit sendRequested(tr("自动测试 #%1").arg(m_sentCount + 1), body);
}

void PeriodicMessageSender::stopSending(const QString &status)
{
    m_timer.stop();
    m_interval->setEnabled(true);
    m_status->setText(status);
    m_toggleButton->setText(tr("开始发送"));
}

void PeriodicMessageSender::updateCount()
{
    m_count->setText(tr("本轮已发送 %1 条").arg(m_sentCount));
}
