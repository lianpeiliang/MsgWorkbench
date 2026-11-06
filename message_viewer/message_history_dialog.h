#ifndef MESSAGE_HISTORY_DIALOG_H
#define MESSAGE_HISTORY_DIALOG_H

#include <QDate>
#include <QDialog>
#include "daily_message_store.h"

class QIntValidator;

namespace Ui
{
class MessageHistoryDialog;
}

class MessageHistoryDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MessageHistoryDialog(const QString &logPath, QWidget *parent = nullptr);
    ~MessageHistoryDialog();
    void ShowTipMsg(const QDate &date);

private slots:
    void on_pushButton_clicked();

    void on_btnDateChoose_clicked();

    void on_pushButton_clearall_clicked();
public:
    bool eventFilter(QObject *watched, QEvent *event) override;
private:
    void SetCurrentPage(int page);
    void showData();
private:
    Ui::MessageHistoryDialog *ui;
    QIntValidator *pageValidator;

    QDate    currentDate;
    DailyMessageStore logdb;

    QPixmap infoPix;

    int m_nCurrentPage;
    int m_nMaxPage;
};

#endif // MESSAGE_HISTORY_DIALOG_H
