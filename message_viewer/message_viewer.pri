QT += widgets sql
CONFIG += c++11
INCLUDEPATH += $$PWD

msvc: QMAKE_CXXFLAGS += /utf-8

SOURCES += \
    $$PWD/message_item_delegate.cpp \
    $$PWD/daily_message_store.cpp \
    $$PWD/messagelist.cpp \
    $$PWD/message_detail_dialog.cpp \
    $$PWD/message_panel.cpp \
    $$PWD/message_history_dialog.cpp

HEADERS += \
    $$PWD/message_item_delegate.h \
    $$PWD/message_item_delegate_p.h \
    $$PWD/daily_message_store.h \
    $$PWD/messagelist.h \
    $$PWD/message_detail_dialog.h \
    $$PWD/message_panel.h \
    $$PWD/message_history_dialog.h

FORMS += \
    $$PWD/message_history_dialog.ui \
    $$PWD/message_detail_dialog.ui \
    $$PWD/message_panel.ui

RESOURCES += $$PWD/message_viewer.qrc
