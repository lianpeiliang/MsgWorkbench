TEMPLATE = app
TARGET = MsgWorkbench

SOURCES += \
    $$PWD/frameless_window.cpp \
    $$PWD/main.cpp \
    $$PWD/message_composer.cpp \
    $$PWD/message_workbench_window.cpp \
    $$PWD/periodic_message_sender.cpp \
    $$PWD/workbench_title_bar.cpp

HEADERS += \
    $$PWD/frameless_window.h \
    $$PWD/message_composer.h \
    $$PWD/message_workbench_window.h \
    $$PWD/periodic_message_sender.h \
    $$PWD/workbench_title_bar.h
RESOURCES += $$PWD/workbench.qrc

include($$PWD/message_viewer/message_viewer.pri)
