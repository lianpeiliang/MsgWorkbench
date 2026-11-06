#ifndef WORKBENCH_TITLE_BAR_H
#define WORKBENCH_TITLE_BAR_H

#include <QFrame>

class QHBoxLayout;

class WorkbenchTitleBar : public QFrame
{
    Q_OBJECT

public:
    explicit WorkbenchTitleBar(QWidget *parent = nullptr);

    // 右侧预留给窗口按钮，具体窗口行为由 FramelessWindow 提供。
    QHBoxLayout *windowControlsLayout() const;

private:
    QHBoxLayout *m_windowControlsLayout;
};

#endif // WORKBENCH_TITLE_BAR_H
