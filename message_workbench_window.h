#ifndef MESSAGE_WORKBENCH_WINDOW_H
#define MESSAGE_WORKBENCH_WINDOW_H

#include "frameless_window.h"

class MessageComposer;
class MessagePanel;

class MessageWorkbenchWindow : public FramelessWindow
{
    Q_OBJECT

public:
    explicit MessageWorkbenchWindow(QWidget *parent = nullptr);

private:
    void submitMessage(const QString &title, const QString &body);

    MessagePanel *m_messages;
    MessageComposer *m_composer;
};

#endif // MESSAGE_WORKBENCH_WINDOW_H
