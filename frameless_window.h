#ifndef FRAMELESS_WINDOW_H
#define FRAMELESS_WINDOW_H

#include <QWidget>

class QFrame;
class QHBoxLayout;
class QPushButton;

class FramelessWindow : public QWidget
{
    Q_OBJECT

public:
    explicit FramelessWindow(QWidget *parent = nullptr);

protected:
    // 将应用自己的页头用作标题栏，并在右侧加入窗口按钮。
    void attachTitleBar(QFrame *titleBar, QHBoxLayout *layout);
    void showEvent(QShowEvent *event) override;
    void changeEvent(QEvent *event) override;
    bool nativeEvent(const QByteArray &eventType, void *message, long *result) override;

private:
    void updateMaximizeButton();

    bool m_customFrame = false;
    QFrame *m_titleBar = nullptr;
    QPushButton *m_maximizeButton = nullptr;
};

#endif // FRAMELESS_WINDOW_H
