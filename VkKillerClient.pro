QT += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET   = VkKillerClient
TEMPLATE = app
CONFIG  += c++14


SOURCES += \
        vkkiller_client.cpp             \
        mainwindow.cpp                  \
        textedit.cpp                    \
        connection_to_host_dialog.cpp   \
        changing_username_dialog.cpp    \
        creating_topic_dialog.cpp       \
        main.cpp

HEADERS  += \
        vkkiller_client.h               \
        vkkiller_request_reply.h        \
        vkkiller_server_constants.h     \
        mainwindow.h                    \
        textedit.h                      \
        connection_to_host_dialog.h     \
        changing_username_dialog.h      \
        creating_topic_dialog.h

FORMS    += \
        mainwindow.ui                   \
        connection_to_host_dialog.ui    \
        changing_username_dialog.ui     \
        creating_topic_dialog.ui

RESOURCES += \
        resources.qrc
