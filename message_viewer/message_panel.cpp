#include "message_panel.h"
#include "ui_message_panel.h"
#include "daily_message_store.h"
#include "message_history_dialog.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include "message_item_delegate.h"

MessagePanel::MessagePanel(QWidget *parent)
    : QWidget(parent),
      ui(new Ui::MessagePanel),
      logPath(QCoreApplication::applicationDirPath() + "/HisTipMsgs")
{
    ui->setupUi(this);
    setAttribute(Qt::WA_StyledBackground);

    auto applyFilter = [this]() {
        ui->messageList->setFilter(ui->msgText->text());
    };
    connect(ui->btnSend, &QPushButton::clicked, this, applyFilter);
    connect(ui->msgText, &QLineEdit::returnPressed, this, applyFilter);
    connect(ui->btnClear, &QPushButton::clicked, ui->messageList, &MessageList::clearAll);
    // 左侧列表从空白开始。只有打开历史窗口时才查询旧记录。
    connect(ui->btnHistory, &QPushButton::clicked, this, [this]() {
        MessageHistoryDialog history(logPath, this);
        history.exec();
    });
}

MessagePanel::~MessagePanel()
{
    delete ui;
}

bool MessagePanel::addMessage(const QString &title, const QString &body, FilterPolicy filterPolicy)
{
    if (body.trimmed().isEmpty() || !QDir().mkpath(logPath))
    {
        return false;
    }

    const auto time = QDateTime::currentDateTime();
    const auto trimmedTitle = title.trimmed();
    const auto messageTitle = trimmedTitle.isEmpty() ? tr("手动消息") : trimmedTitle;
    constexpr int infoMessageType = 0;

    DailyMessageStore logdb;
    logdb.setDbPath(logPath);
    if (!logdb.addMsg(infoMessageType, body, messageTitle, time))
    {
        return false;
    }

    if (filterPolicy == FilterPolicy::RevealNewMessage)
    {
        ui->msgText->clear();
        ui->messageList->setFilter(QString());
    }
    ui->messageList->addMessage(body, QPixmap(":/message_viewer/icon_info.png"), time, messageTitle, false);
    return true;
}

void MessagePanel::on_btnPrint_clicked()
{
    const QString filename = QFileDialog::getSaveFileName(
        this, tr("保存消息"), QString(), tr("文本文件 (*.txt)"));
    if (filename.isEmpty())
    {
        return;
    }

    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QMessageBox::warning(this, tr("保存消息"), file.errorString());
        return;
    }

    QTextStream out(&file);
    // 从筛选代理读取数据，导出内容与当前列表显示一致。
    const auto *model = ui->messageList->model();
    for (int row = 0; row < model->rowCount(); ++row)
    {
        const auto index = model->index(row, 0);
        out << index.data(DateTimeRole).toString() << '\n'
            << index.data(TitleContextRole).toString() << '\n'
            << index.data().toString() << "\n\n";
    }
    out.flush();
    if (out.status() != QTextStream::Ok)
    {
        QMessageBox::warning(this, tr("保存消息"), tr("无法完整写入文件。"));
    }
}
