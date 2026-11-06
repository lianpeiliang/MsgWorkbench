#include "message_history_dialog.h"
#include "ui_message_history_dialog.h"
#include <QKeyEvent>
#include <QMouseEvent>
#include <QCalendarWidget>
#include <QDialogButtonBox>
#include <QIntValidator>
#include <QMessageBox>
#include <QVBoxLayout>

// 每页显示数量
#define MAX_PER_PAGE 50

MessageHistoryDialog::MessageHistoryDialog(const QString &logPath, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MessageHistoryDialog),
    pageValidator(new QIntValidator(1, 1, this)),
    infoPix(":/message_viewer/icon_info.png"),
    m_nCurrentPage(0),
    m_nMaxPage(0)
{
    ui->setupUi(this);

    logdb.setDbPath(logPath);
    ui->Line_CurrentPage->setValidator(pageValidator);

    // 初始化翻页
    ui->label_Pre->setProperty("page", "true");
    ui->label_Next->setProperty("page", "true");
    ui->label_First->setProperty("page", "true");
    ui->label_Last->setProperty("page", "true");

    ui->label_Pre->installEventFilter(this);
    ui->label_Next->installEventFilter(this);
    ui->label_First->installEventFilter(this);
    ui->label_Last->installEventFilter(this);

    ui->Line_CurrentPage->installEventFilter(this);
    ui->Line_CurrentPage->setEnabled(false);
    // 显示当天数据
    ShowTipMsg(QDate::currentDate());
}

MessageHistoryDialog::~MessageHistoryDialog()
{
    delete ui;
}

void MessageHistoryDialog::ShowTipMsg(const QDate &date)
{
    ui->lblDate->setText(date.toString("yyyy-MM-dd"));
    currentDate = date;
    // 先重置旧日期的列表和分页状态，确保无数据时不会保留旧页码。
    ui->messageList->clearAll();
    m_nCurrentPage = 0;
    m_nMaxPage = 0;
    ui->Line_CurrentPage->clear();
    ui->Line_CurrentPage->setEnabled(false);
    ui->label_First->setEnabled(false);
    ui->label_Pre->setEnabled(false);
    ui->label_Last->setEnabled(false);
    ui->label_Next->setEnabled(false);
    ui->label_TotalPage->setText(QString::number(m_nMaxPage));
    // 判断日期日志是否存在
    if (!logdb.exist(date))
    {
        ui->label_tip->setVisible(true);
        return;
    }
    if (!logdb.open(date))
    {
        ui->label_tip->setVisible(true);
        return;
    }
    ui->label_tip->setVisible(false);

    // 初始化页数
    auto count = logdb.count();
    if (count % MAX_PER_PAGE == 0)
    {
        m_nMaxPage = count / MAX_PER_PAGE;
    }
    else
    {
        m_nMaxPage = count / MAX_PER_PAGE + 1;
    }
    if (0 == m_nMaxPage)
    {
        m_nMaxPage = 1;
    }

    // 页码输入与翻页按钮共用当前日期的页数上限。
    pageValidator->setTop(m_nMaxPage);
    ui->Line_CurrentPage->setText(QString::number(m_nCurrentPage));
    ui->label_TotalPage->setText(QString::number(m_nMaxPage));
    ui->Line_CurrentPage->setEnabled(true);
    m_nCurrentPage = 0;
    SetCurrentPage(1);
}

void MessageHistoryDialog::on_pushButton_clicked()
{
    if (QMessageBox::question(this, tr("删除记录"),
                              tr("是否删除 %1 的记录？").arg(currentDate.toString("yyyy-MM-dd")),
                              QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::Yes)
    {
        if (!logdb.remove(currentDate))
        {
            QMessageBox::warning(this, tr("删除记录"), tr("无法删除该日期的记录。"));
        }
        ShowTipMsg(currentDate);
    }
}

void MessageHistoryDialog::on_pushButton_clearall_clicked()
{
    if (QMessageBox::question(this, tr("删除记录"), tr("是否删除所有记录？"),
                              QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::Yes)
    {
        if (!logdb.removeall())
        {
            QMessageBox::warning(this, tr("删除记录"), tr("部分记录无法删除。"));
        }
        ShowTipMsg(currentDate);
    }
}

void MessageHistoryDialog::on_btnDateChoose_clicked()
{
    QDialog dialog(this);
    dialog.setWindowTitle(tr("选择日期"));

    auto *layout = new QVBoxLayout(&dialog);
    auto *calendar = new QCalendarWidget(&dialog);
    calendar->setSelectedDate(currentDate);
    calendar->setFirstDayOfWeek(Qt::Monday);
    calendar->setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);
    layout->addWidget(calendar);

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    layout->addWidget(buttons);
    connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    connect(calendar, &QCalendarWidget::activated, &dialog, &QDialog::accept);
    if (dialog.exec() == QDialog::Accepted)
    {
        ShowTipMsg(calendar->selectedDate());
    }
}

bool MessageHistoryDialog::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonRelease)
    {
        // 翻页点击响应
        QMouseEvent *pEvent = (QMouseEvent *)event;
        if (pEvent->button() != Qt::LeftButton)
        {
            return QDialog::eventFilter(watched, event);
        }
        int page = -1;
        if (watched == ui->label_Pre)   // 前一页
        {
            page = m_nCurrentPage - 1;
        }
        else if (watched == ui->label_Next) // 后一页
        {
            page = m_nCurrentPage + 1;
        }
        else if (watched == ui->label_First)    // 第一页
        {
            page = 1;
        }
        else if (watched == ui->label_Last)     // 最后一页
        {
            page = m_nMaxPage;
        }
        if (page > 0)
        {
            SetCurrentPage(page);
            return true;
        }
    }
    else if (event->type() == QEvent::KeyPress && watched == ui->Line_CurrentPage)
    {
        const auto *keyEvent = static_cast<QKeyEvent *>(event);
        if (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter)
        {
            const int page = ui->Line_CurrentPage->text().toInt();
            if (page >= 1 && page <= m_nMaxPage)
            {
                SetCurrentPage(page);
            }
            return true;
        }
    }
    return QDialog::eventFilter(watched, event);
}

void MessageHistoryDialog::SetCurrentPage(int page)
{
    int tmppage = page > m_nMaxPage ? m_nMaxPage : page;
    tmppage = tmppage < 1 ? 1 : tmppage;
    if (m_nCurrentPage == tmppage)
    {
        return;
    }
    m_nCurrentPage = tmppage;
    ui->Line_CurrentPage->setText(QString::number(m_nCurrentPage));

    // 设置各个翻页控件的状态
    if (1 == m_nCurrentPage)
    {
        // 第一页前翻禁用
        ui->label_First->setEnabled(false);
        ui->label_Pre->setEnabled(false);
    }
    else
    {
        ui->label_First->setEnabled(true);
        ui->label_Pre->setEnabled(true);
    }
    if (m_nCurrentPage == m_nMaxPage)
    {
        // 最后页后翻禁用
        ui->label_Last->setEnabled(false);
        ui->label_Next->setEnabled(false);
    }
    else
    {
        ui->label_Last->setEnabled(true);
        ui->label_Next->setEnabled(true);
    }
    if (1 == m_nMaxPage)
    {
        // 只有一页时禁用页数输入
        ui->Line_CurrentPage->setEnabled(false);
    }
    else
    {
        ui->Line_CurrentPage->setEnabled(true);
    }
    showData();
}

void MessageHistoryDialog::showData()
{
    // 界面页码从 1 开始，数据库偏移量从 0 开始。
    ui->messageList->clearAll();
    auto infos = logdb.getData((m_nCurrentPage - 1) * MAX_PER_PAGE, MAX_PER_PAGE);
    for (const auto &record : infos)
    {
        ui->messageList->addMessage(record.info, infoPix, record.dt, record.title, false);
    }
}
