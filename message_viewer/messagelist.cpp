/**
MIT License

Copyright (c) 2018 Michael Scopchanov

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "messagelist.h"
#include "message_item_delegate.h"
#include <QStandardItemModel>
#include <QDateTime>
#include <QPainter>
#include <QSortFilterProxyModel>

MessageList::MessageList(QWidget *parent) :
    QListView(parent)
{
    auto *delegate = new MessageItemDelegate(this);

    delegate->setContentsMargins(14, 11, 14, 11);
    delegate->setIconSize(20, 20);
    delegate->setHorizontalSpacing(10);
    delegate->setVerticalSpacing(6);

    setAlternatingRowColors(true);
    setResizeMode(QListView::Adjust);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    pModel = new QStandardItemModel(this);
    pFilterModel = new QSortFilterProxyModel(this);
    pFilterModel->setSourceModel(pModel);
    // 第 0 列的显示文本是消息正文，标题和时间通过自定义角色存放。
    pFilterModel->setFilterKeyColumn(0);
    setModel(pFilterModel);
    setItemDelegate(delegate);

    this->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
}

void MessageList::changeColor(bool bBlack)
{
    QPalette p(palette());
    //  使用样式qtableview中的颜色
    if (bBlack)
    {
        // dark
        p.setBrush(QPalette::WindowText, QColor(200, 200, 200));
        p.setBrush(QPalette::Base, QColor(35, 35, 35));
        p.setBrush(QPalette::AlternateBase, QColor(42, 42, 42));
        // 线条颜色  gridline-color
        p.setBrush(QPalette::Mid, QColor(63, 63, 63));
        p.setBrush(QPalette::Dark, QColor(63, 63, 63));
        // 文字
        p.setBrush(QPalette::Text, QColor(200, 200, 200));
        // 背景色 未选中 QTableView::item背景色
        p.setBrush(QPalette::PlaceholderText, QColor(175, 175, 175));
        // 背景色 选中 QTableView::item:selected背景色
        p.setBrush(QPalette::Highlight, QColor(60, 60, 60));
        p.setBrush(QPalette::HighlightedText, QColor(255, 255, 255));
        // 详细文字
        p.setBrush(QPalette::Link, QColor(18, 207, 216));
        // 标题/时间
        p.setBrush(QPalette::Midlight, QColor(18, 207, 216));
    }
    else
    {
        // blue
        p.setBrush(QPalette::WindowText, QColor(255, 255, 255));
        p.setBrush(QPalette::Base, QColor(35, 75, 115));
        p.setBrush(QPalette::AlternateBase, QColor(42, 84, 125));
        // 线条颜色  gridline-color
        p.setBrush(QPalette::Mid, QColor(50, 100, 150));
        p.setBrush(QPalette::Dark, QColor(50, 100, 150));
        // 文字
        p.setBrush(QPalette::Text, QColor(255, 255, 255));
        // 背景色 未选中 QTableView::item背景色
        p.setBrush(QPalette::PlaceholderText, QColor(202, 218, 235));
        // 背景色 选中 QTableView::item:selected背景色
        p.setBrush(QPalette::Highlight, QColor(50, 100, 145));
        p.setBrush(QPalette::HighlightedText, QColor(255, 255, 255));
        // 详细文字
        p.setBrush(QPalette::Link, QColor(18, 207, 216));
        // 标题/时间
        p.setBrush(QPalette::Midlight, QColor(18, 207, 216));
    }
    setPalette(p);
}

void MessageList::setFilter(const QString &strText)
{
    auto tmp = strText.trimmed();
    if (tmp.isEmpty())
    {
        pFilterModel->setFilterWildcard(QString());
    }
    else
    {
        pFilterModel->setFilterWildcard(QString("*%1*").arg(tmp));
    }
}

void MessageList::addMessage(const QString &text, const QPixmap &pixmap,
                             const QDateTime &dateTime, const QString &title, bool bIsErrorInfo)
{
    auto *item = new QStandardItem(QIcon(pixmap), text);

    // 列表达到上限时移除最早的条目。数据库中的历史记录仍可查询。
    if (pModel->rowCount() >= MaxMessageCount)
    {
        pModel->removeRow(0);
    }

    item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    // 只显示时间
    item->setData(dateTime.toString("hh:mm:ss"), DateTimeRole);
    item->setData(title, TitleContextRole);
    item->setData(bIsErrorInfo, ErrorInfoFlagRole);
    pModel->appendRow(item);
    scrollToBottom();
}

void MessageList::clearAll()
{
    pModel->clear();
}

void MessageList::paintEvent(QPaintEvent *event)
{
    QListView::paintEvent(event);
    if (pFilterModel->rowCount() == 0)
    {
        QPainter painter(viewport());
        painter.setPen(palette().placeholderText().color());
        painter.drawText(viewport()->rect(), Qt::AlignCenter, tr("暂无可显示的消息"));
    }
}

