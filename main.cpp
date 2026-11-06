#include <QApplication>
#include <QFile>
#include <QPalette>
#include "message_workbench_window.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("MsgWorkbench"));
    app.setStyle(QStringLiteral("Fusion"));
    QFont font = app.font();
    font.setPointSize(10);
    app.setFont(font);

    QPalette palette = app.palette();
    palette.setColor(QPalette::Window, QColor("#101520"));
    palette.setColor(QPalette::WindowText, QColor("#edf3fc"));
    palette.setColor(QPalette::Base, QColor("#121a29"));
    palette.setColor(QPalette::AlternateBase, QColor("#162032"));
    palette.setColor(QPalette::Text, QColor("#e2eaf7"));
    palette.setColor(QPalette::Button, QColor("#202c40"));
    palette.setColor(QPalette::ButtonText, QColor("#e2eaf7"));
    palette.setColor(QPalette::Mid, QColor("#2b3a53"));
    palette.setColor(QPalette::Link, QColor("#76e2dd"));
    palette.setColor(QPalette::Highlight, QColor("#21445b"));
    palette.setColor(QPalette::HighlightedText, QColor("#f1fcff"));
    palette.setColor(QPalette::PlaceholderText, QColor("#8295af"));
    palette.setColor(QPalette::Disabled, QPalette::Text, QColor("#7d8da5"));
    palette.setColor(QPalette::Disabled, QPalette::ButtonText, QColor("#7d8da5"));
    app.setPalette(palette);

    QFile style(QStringLiteral(":/workbench/workbench.qss"));
    if (style.open(QIODevice::ReadOnly))
    {
        app.setStyleSheet(QString::fromUtf8(style.readAll()));
    }

    MessageWorkbenchWindow window;
    window.show();
    return app.exec();
}
