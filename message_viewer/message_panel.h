#ifndef MESSAGE_PANEL_H
#define MESSAGE_PANEL_H

#include <QWidget>

namespace Ui
{
class MessagePanel;
}

class MessagePanel : public QWidget
{
    Q_OBJECT

public:
    enum class FilterPolicy { RevealNewMessage, KeepCurrent };

    explicit MessagePanel(QWidget *parent = nullptr);
    ~MessagePanel();

    // 先保存到当天的历史库，成功后才追加到当前列表。
    bool addMessage(const QString &title, const QString &body,
                    FilterPolicy filterPolicy = FilterPolicy::RevealNewMessage);

private slots:
    void on_btnPrint_clicked();

private:
    Ui::MessagePanel *ui;
    const QString logPath;
};

#endif // MESSAGE_PANEL_H
