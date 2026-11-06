#include "frameless_window.h"

#include <QEvent>
#include <QFrame>
#include <QGuiApplication>
#include <QHBoxLayout>
#include <QPainter>
#include <QPushButton>

#ifdef Q_OS_WIN
#include <qt_windows.h>
#include <windowsx.h>
#endif

namespace
{
class WindowButton : public QPushButton
{
public:
    enum class Action { Minimize, Maximize, Close };

    WindowButton(Action action, QWidget *parent)
        : QPushButton(parent), m_action(action)
    {
        setProperty("windowControl", true);
        setFixedSize(36, 30);
    }

protected:
    void paintEvent(QPaintEvent *event) override
    {
        QPushButton::paintEvent(event);
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.translate(width() / 2.0, height() / 2.0);
        painter.setPen(QPen(palette().buttonText().color(), 1.3));

        switch (m_action)
        {
        case Action::Minimize:
            painter.drawLine(QPointF(-5, 3), QPointF(5, 3));
            break;
        case Action::Maximize:
            if (window()->isMaximized())
            {
                painter.drawRect(QRectF(-5, -2, 7, 7));
                painter.drawLine(QPointF(-2, -2), QPointF(-2, -5));
                painter.drawLine(QPointF(-2, -5), QPointF(5, -5));
                painter.drawLine(QPointF(5, -5), QPointF(5, 2));
                painter.drawLine(QPointF(5, 2), QPointF(2, 2));
            }
            else
            {
                painter.drawRect(QRectF(-5, -5, 10, 10));
            }
            break;
        case Action::Close:
            painter.drawLine(QPointF(-5, -5), QPointF(5, 5));
            painter.drawLine(QPointF(-5, 5), QPointF(5, -5));
            break;
        }
    }

private:
    Action m_action;
};
}

FramelessWindow::FramelessWindow(QWidget *parent)
    : QWidget(parent)
{
#ifdef Q_OS_WIN
    m_customFrame = isWindow() && QGuiApplication::platformName() == QStringLiteral("windows");
    if (m_customFrame)
    {
        setWindowFlag(Qt::FramelessWindowHint);
    }
#endif
}

void FramelessWindow::attachTitleBar(QFrame *titleBar, QHBoxLayout *layout)
{
    if (!m_customFrame)
    {
        return;
    }

    m_titleBar = titleBar;
    auto *minimize = new WindowButton(WindowButton::Action::Minimize, titleBar);
    minimize->setAccessibleName(tr("最小化"));
    minimize->setToolTip(tr("最小化"));
    m_maximizeButton = new WindowButton(WindowButton::Action::Maximize, titleBar);
    auto *close = new WindowButton(WindowButton::Action::Close, titleBar);
    close->setObjectName(QStringLiteral("closeWindowButton"));
    close->setAccessibleName(tr("关闭"));
    close->setToolTip(tr("关闭"));

    layout->addSpacing(8);
    layout->addWidget(minimize);
    layout->addWidget(m_maximizeButton);
    layout->addWidget(close);

    connect(minimize, &QPushButton::clicked, this, &QWidget::showMinimized);
    connect(m_maximizeButton, &QPushButton::clicked, this, [this]() {
        if (isMaximized())
        {
            showNormal();
        }
        else
        {
            showMaximized();
        }
    });
    connect(close, &QPushButton::clicked, this, &QWidget::close);
    updateMaximizeButton();
}

void FramelessWindow::updateMaximizeButton()
{
    if (m_maximizeButton)
    {
        const QString action = isMaximized() ? tr("还原") : tr("最大化");
        m_maximizeButton->setAccessibleName(action);
        m_maximizeButton->setToolTip(action);
        m_maximizeButton->update();
    }
}

void FramelessWindow::changeEvent(QEvent *event)
{
    QWidget::changeEvent(event);
    if (event->type() == QEvent::WindowStateChange)
    {
        updateMaximizeButton();
    }
}

void FramelessWindow::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
#ifdef Q_OS_WIN
    if (m_customFrame)
    {
        const auto handle = reinterpret_cast<HWND>(winId());
        const LONG_PTR style = GetWindowLongPtr(handle, GWL_STYLE);
        const LONG_PTR flags = WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU;
        if ((style & flags) != flags)
        {
            // 保留系统的拖动、贴边和缩放能力，标题栏仍由 Qt 绘制。
            SetWindowLongPtr(handle, GWL_STYLE, style | flags);
            SetWindowPos(handle, nullptr, 0, 0, 0, 0,
                         SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
        }
    }
#endif
}

bool FramelessWindow::nativeEvent(const QByteArray &eventType, void *message, long *result)
{
#ifdef Q_OS_WIN
    if (!m_customFrame || eventType != "windows_generic_MSG")
    {
        return QWidget::nativeEvent(eventType, message, result);
    }

    const auto *msg = static_cast<const MSG *>(message);
    switch (msg->message)
    {
    case WM_NCCALCSIZE:
        // 整个窗口都作为 Qt 客户区，避免系统额外画出一圈边框。
        *result = 0;
        return true;
    case WM_NCACTIVATE:
        if (!IsIconic(msg->hwnd))
        {
            *result = static_cast<long>(DefWindowProc(msg->hwnd, msg->message, msg->wParam, -1));
            return true;
        }
        break;
    case WM_NCPAINT:
        if (!IsIconic(msg->hwnd))
        {
            *result = 0;
            return true;
        }
        break;
    case WM_GETMINMAXINFO:
    {
        MONITORINFO monitor = {};
        monitor.cbSize = sizeof(monitor);
        if (GetMonitorInfo(MonitorFromWindow(msg->hwnd, MONITOR_DEFAULTTONEAREST), &monitor))
        {
            auto *info = reinterpret_cast<MINMAXINFO *>(msg->lParam);
            // 最大化使用当前屏幕的工作区，给任务栏留出位置。
            info->ptMaxPosition.x = monitor.rcWork.left - monitor.rcMonitor.left;
            info->ptMaxPosition.y = monitor.rcWork.top - monitor.rcMonitor.top;
            info->ptMaxSize.x = monitor.rcWork.right - monitor.rcWork.left;
            info->ptMaxSize.y = monitor.rcWork.bottom - monitor.rcWork.top;
            const QSize minimum = minimumSize().expandedTo(minimumSizeHint());
            info->ptMinTrackSize.x = qMax(info->ptMinTrackSize.x,
                                         static_cast<LONG>(qRound(minimum.width() * devicePixelRatioF())));
            info->ptMinTrackSize.y = qMax(info->ptMinTrackSize.y,
                                         static_cast<LONG>(qRound(minimum.height() * devicePixelRatioF())));
            *result = 0;
            return true;
        }
        break;
    }
    case WM_NCHITTEST:
    {
        RECT bounds;
        if (!m_titleBar || !GetWindowRect(msg->hwnd, &bounds))
        {
            break;
        }

        const int x = GET_X_LPARAM(msg->lParam);
        const int y = GET_Y_LPARAM(msg->lParam);
        const qreal scale = devicePixelRatioF();
        const int grip = qMax(6, qRound(6 * scale));
        const bool left = x < bounds.left + grip;
        const bool right = x >= bounds.right - grip;
        const bool top = y < bounds.top + grip;
        const bool bottom = y >= bounds.bottom - grip;
        if (!isMaximized() && (left || right || top || bottom))
        {
            *result = top ? (left ? HTTOPLEFT : right ? HTTOPRIGHT : HTTOP)
                          : bottom ? (left ? HTBOTTOMLEFT : right ? HTBOTTOMRIGHT : HTBOTTOM)
                                   : left ? HTLEFT : HTRIGHT;
            return true;
        }

        const QPoint local(qRound((x - bounds.left) / scale), qRound((y - bounds.top) / scale));
        if (m_titleBar->rect().contains(m_titleBar->mapFrom(this, local)))
        {
            for (QWidget *child = childAt(local); child && child != m_titleBar; child = child->parentWidget())
            {
                if (qobject_cast<QPushButton *>(child))
                {
                    *result = HTCLIENT;
                    return true;
                }
            }
            // 交给 Windows 处理拖动、双击最大化和贴边吸附。
            *result = HTCAPTION;
            return true;
        }
        *result = HTCLIENT;
        return true;
    }
    default:
        break;
    }
#endif
    return QWidget::nativeEvent(eventType, message, result);
}
