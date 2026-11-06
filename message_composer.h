#ifndef MESSAGE_COMPOSER_H
#define MESSAGE_COMPOSER_H

#include <QFrame>

class QLineEdit;
class QPlainTextEdit;

class MessageComposer : public QFrame
{
    Q_OBJECT

public:
    explicit MessageComposer(QWidget *parent = nullptr);
    void reset();

signals:
    void submitRequested(const QString &title, const QString &body);

private:
    QLineEdit *m_title;
    QPlainTextEdit *m_body;
};

#endif // MESSAGE_COMPOSER_H
