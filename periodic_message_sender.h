#ifndef PERIODIC_MESSAGE_SENDER_H
#define PERIODIC_MESSAGE_SENDER_H

#include <QFrame>
#include <QTimer>

class QLabel;
class QPlainTextEdit;
class QPushButton;
class QSpinBox;

class PeriodicMessageSender : public QFrame
{
    Q_OBJECT

public:
    explicit PeriodicMessageSender(QWidget *parent = nullptr);
    void reportResult(bool saved);

signals:
    void sendRequested(const QString &title, const QString &body);

private:
    void toggleSending();
    void sendNext();
    void stopSending(const QString &status);
    void updateCount();

    QTimer m_timer;
    QSpinBox *m_interval;
    QPlainTextEdit *m_body;
    QLabel *m_status;
    QLabel *m_count;
    QPushButton *m_toggleButton;
    qint64 m_sentCount = 0;
};

#endif // PERIODIC_MESSAGE_SENDER_H
