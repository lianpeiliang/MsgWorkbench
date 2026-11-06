TEMPLATE = app
TARGET = MsgWorkbench

SOURCES += \
    $$PWD/main.cpp \
    $$PWD/message_composer.cpp \
    $$PWD/message_workbench_window.cpp \
    $$PWD/periodic_message_sender.cpp

HEADERS += \
    $$PWD/message_composer.h \
    $$PWD/message_workbench_window.h \
    $$PWD/periodic_message_sender.h
RESOURCES += $$PWD/workbench.qrc

include($$PWD/message_viewer/message_viewer.pri)
