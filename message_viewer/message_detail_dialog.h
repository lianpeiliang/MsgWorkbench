#ifndef MESSAGE_DETAIL_DIALOG_H
#define MESSAGE_DETAIL_DIALOG_H

#include <QDialog>


namespace Ui
{
class MessageDetailDialog;
}

class MessageDetailDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MessageDetailDialog(QWidget *parent = nullptr);
    ~MessageDetailDialog();

    void setText(const QString &text);

protected slots:
    void on_closeBtn_clicked();

private:
    Ui::MessageDetailDialog *ui;
};

#endif // MESSAGE_DETAIL_DIALOG_H
