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

#include "message_item_delegate.h"
#include "message_item_delegate_p.h"
#include <QPainter>
#include <QAbstractItemView>
#include <QMouseEvent>
#include "message_detail_dialog.h"
#include <QTextLayout>
#include <QtMath>


MessageItemDelegate::MessageItemDelegate(QObject *parent) :
    QStyledItemDelegate(parent),
    m_ptr(new MessageItemDelegatePrivate)
{

}

MessageItemDelegate::~MessageItemDelegate()
{
    delete m_ptr;
}

QSize MessageItemDelegate::iconSize() const
{
    return m_ptr->iconSize;
}

void MessageItemDelegate::setIconSize(int width, int height)
{
    m_ptr->iconSize = QSize(width, height);
}

QMargins MessageItemDelegate::contentsMargins() const
{
    return m_ptr->margins;
}

void MessageItemDelegate::setContentsMargins(int left, int top, int right, int bottom)
{
    m_ptr->margins = QMargins(left, top, right, bottom);
}

int MessageItemDelegate::horizontalSpacing() const
{
    return m_ptr->spacingHorizontal;
}

void MessageItemDelegate::setHorizontalSpacing(int spacing)
{
    m_ptr->spacingHorizontal = spacing;
}

int MessageItemDelegate::verticalSpacing() const
{
    return m_ptr->spacingVertical;
}

void MessageItemDelegate::setVerticalSpacing(int spacing)
{
    m_ptr->spacingVertical = spacing;
}

namespace
{
constexpr int MaxPreviewLines = 10;

QString previewText(QString text)
{
    text.replace(QStringLiteral("\r\n"), QStringLiteral("\n"));
    text.replace(QLatin1Char('\r'), QLatin1Char('\n'));
    // QTextLayout 使用 Unicode 行分隔符处理正文里的手动换行。
    return text.replace(QLatin1Char('\n'), QChar(QChar::LineSeparator));
}

QString detailLinkText()
{
    return MessageItemDelegate::tr("查看全文");
}
}

struct MessageItemDelegatePrivate::RowLayout
{
    RowLayout(const QStyleOptionViewItem &option, const QModelIndex &index, int rowWidth,
              const MessageItemDelegatePrivate &settings)
        : body(previewText(index.data(Qt::DisplayRole).toString()), option.font,
               const_cast<QWidget *>(option.widget)),
          titleFont(option.font)
    {
        const QMargins &margins = settings.margins;
        const int textLeft = margins.left() + settings.iconSize.width() + settings.spacingHorizontal;
        const int textWidth = qMax(1, rowWidth - textLeft - margins.right());
        auto *device = const_cast<QWidget *>(option.widget);
        const QFontMetrics metrics(option.font, device);
        titleFont.setBold(true);
        const int headerHeight = qMax(metrics.height(), QFontMetrics(titleFont, device).height());
        const int timeWidth = qMin(textWidth, metrics.horizontalAdvance(index.data(DateTimeRole).toString()));
        timestamp = QRect(textLeft + textWidth - timeWidth, margins.top(), timeWidth, headerHeight);
        title = QRect(textLeft, margins.top(),
                      qMax(0, textWidth - timeWidth - settings.spacingHorizontal), headerHeight);

        QTextOption textOption;
        textOption.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
        body.setTextOption(textOption);
        bodyOrigin = QPoint(textLeft, margins.top() + headerHeight + settings.spacingVertical);

        qreal bodyHeight = 0;
        const int textLength = body.text().size();
        body.beginLayout();
        for (int i = 0; i < MaxPreviewLines; ++i)
        {
            QTextLine line = body.createLine();
            if (!line.isValid())
            {
                break;
            }
            line.setLineWidth(textWidth);
            line.setPosition(QPointF(0, bodyHeight));
            bodyHeight += line.height();
            truncated = line.textStart() + line.textLength() < textLength;
        }
        body.endLayout();

        int textBottom = bodyOrigin.y() + qCeil(bodyHeight);
        if (truncated)
        {
            // 详情入口单独占一行，避免覆盖正文最后一行。
            const int linkWidth = qMin(textWidth, metrics.horizontalAdvance(detailLinkText()));
            detail = QRect(textLeft + textWidth - linkWidth, textBottom + settings.spacingVertical,
                           linkWidth, metrics.height());
            textBottom = detail.bottom() + 1;
        }
        height = qMax(margins.top() + settings.iconSize.height(), textBottom) + margins.bottom();
    }

    QTextLayout body;
    QFont titleFont;
    QPoint bodyOrigin;
    QRect title;
    QRect timestamp;
    QRect detail;
    int height = 0;
    bool truncated = false;
};

void MessageItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                                const QModelIndex &index) const
{
    QStyleOptionViewItem opt(option);
    initStyleOption(&opt, index);
    const MessageItemDelegatePrivate::RowLayout layout(opt, index, opt.rect.width(), *m_ptr);

    const QPalette &palette = opt.palette;
    const bool selected = opt.state & QStyle::State_Selected;
    const bool alternate = opt.features & QStyleOptionViewItem::Alternate;
    const QColor background = selected ? palette.highlight().color()
        : (alternate ? palette.alternateBase().color() : palette.base().color());
    const QColor foreground = selected ? palette.highlightedText().color() : palette.text().color();

    painter->save();
    painter->setClipRect(opt.rect, Qt::IntersectClip);
    painter->translate(opt.rect.topLeft());
    const QRect rowRect(QPoint(0, 0), opt.rect.size());
    painter->fillRect(rowRect, background);
    if (selected)
    {
        painter->fillRect(QRect(0, 0, 3, rowRect.height()), palette.link());
    }
    if (index.row() + 1 < index.model()->rowCount())
    {
        painter->setPen(palette.mid().color());
        painter->drawLine(m_ptr->margins.left(), rowRect.bottom(),
                          rowRect.right() - m_ptr->margins.right(), rowRect.bottom());
    }

    if (!opt.icon.isNull())
    {
        const QRect iconRect(QPoint(m_ptr->margins.left(), m_ptr->margins.top()), m_ptr->iconSize);
        opt.icon.paint(painter, iconRect);
    }

    painter->setFont(opt.font);
    painter->setPen(selected ? foreground : palette.placeholderText().color());
    painter->drawText(layout.timestamp, Qt::AlignRight | Qt::AlignVCenter | Qt::TextSingleLine,
                      index.data(DateTimeRole).toString());
    if (!layout.title.isEmpty())
    {
        painter->setFont(layout.titleFont);
        painter->setPen(foreground);
        const QString title = painter->fontMetrics().elidedText(index.data(TitleContextRole).toString(),
                                                                Qt::ElideRight, layout.title.width());
        painter->drawText(layout.title, Qt::AlignVCenter | Qt::TextSingleLine, title);
    }

    painter->setFont(opt.font);
    painter->setPen(index.data(ErrorInfoFlagRole).toBool() ? QColor("#e25a76") : foreground);
    layout.body.draw(painter, layout.bodyOrigin);
    if (!layout.detail.isEmpty())
    {
        painter->setPen(palette.link().color());
        painter->drawText(layout.detail, Qt::AlignRight | Qt::AlignVCenter | Qt::TextSingleLine,
                          detailLinkText());
    }
    painter->restore();
}

QSize MessageItemDelegate::sizeHint(const QStyleOptionViewItem &option,
                                   const QModelIndex &index) const
{
    QStyleOptionViewItem opt(option);
    initStyleOption(&opt, index);
    // 列表测量时可能还没有行矩形，此时以实际视口宽度计算换行。
    const auto *view = qobject_cast<const QAbstractItemView *>(parent());
    const int width = view ? view->viewport()->width() : qMax(1, opt.rect.width());
    const MessageItemDelegatePrivate::RowLayout layout(opt, index, width, *m_ptr);
    return QSize(width, layout.height);
}

bool MessageItemDelegate::editorEvent(QEvent *event,
                                      QAbstractItemModel *model,
                                      const QStyleOptionViewItem &option,
                                      const QModelIndex &index)
{
    if (event->type() == QEvent::MouseButtonRelease)
    {
        const auto *mouseEvent = static_cast<QMouseEvent *>(event);
        if (mouseEvent->button() == Qt::LeftButton)
        {
            QStyleOptionViewItem opt(option);
            initStyleOption(&opt, index);
            const MessageItemDelegatePrivate::RowLayout layout(opt, index, opt.rect.width(), *m_ptr);
            if (layout.detail.contains(mouseEvent->pos() - opt.rect.topLeft()))
            {
                auto *dialog = new MessageDetailDialog(const_cast<QWidget *>(opt.widget));
                dialog->setAttribute(Qt::WA_DeleteOnClose);
                dialog->setText(index.data(Qt::DisplayRole).toString());
                dialog->show();
                return true;
            }
        }
    }
    return QStyledItemDelegate::editorEvent(event, model, option, index);
}

MessageItemDelegatePrivate::MessageItemDelegatePrivate() :
    iconSize(24, 24),
    margins(0, 0, 0, 0),
    spacingHorizontal(0),
    spacingVertical(0)
{
}
