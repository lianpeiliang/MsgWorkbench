#include "message_detail_dialog.h"
#include "ui_message_detail_dialog.h"

MessageDetailDialog::MessageDetailDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MessageDetailDialog)
{
    ui->setupUi(this);
}

MessageDetailDialog::~MessageDetailDialog()
{
    delete ui;
}

void MessageDetailDialog::setText(const QString &text)
{
    ui->teText->setPlainText(text);
}

void MessageDetailDialog::on_closeBtn_clicked()
{
    this->close();
}
