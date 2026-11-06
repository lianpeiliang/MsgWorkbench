#include "message_composer.h"

#include <QLabel>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QVBoxLayout>

MessageComposer::MessageComposer(QWidget *parent)
    : QFrame(parent),
      m_title(new QLineEdit(this)),
      m_body(new QPlainTextEdit(this))
{
    setObjectName(QStringLiteral("composerPanel"));
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(18, 16, 18, 16);
    layout->setSpacing(10);

    auto *heading = new QLabel(tr("撰写消息"), this);
    heading->setProperty("sectionHeading", true);
    layout->addWidget(heading);

    auto *titleLabel = new QLabel(tr("标题"), this);
    titleLabel->setBuddy(m_title);
    m_title->setPlaceholderText(tr("可选"));
    layout->addWidget(titleLabel);
    layout->addWidget(m_title);

    auto *bodyLabel = new QLabel(tr("正文"), this);
    bodyLabel->setBuddy(m_body);
    m_body->setPlaceholderText(tr("请输入消息正文"));
    layout->addWidget(bodyLabel);
    layout->addWidget(m_body, 1);

    auto *addButton = new QPushButton(tr("添加消息"), this);
    addButton->setObjectName(QStringLiteral("addMessageButton"));
    addButton->setCursor(Qt::PointingHandCursor);
    addButton->setEnabled(false);
    layout->addWidget(addButton, 0, Qt::AlignRight);

    connect(m_body, &QPlainTextEdit::textChanged, addButton, [this, addButton]() {
        addButton->setEnabled(!m_body->toPlainText().trimmed().isEmpty());
    });
    connect(addButton, &QPushButton::clicked, this, [this]() {
        emit submitRequested(m_title->text(), m_body->toPlainText());
    });
}

void MessageComposer::reset()
{
    m_title->clear();
    m_body->clear();
    m_body->setFocus();
}
