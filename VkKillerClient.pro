#-------------------------------------------------
#
# Project created by QtCreator 2018-02-03T12:48:43
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = VkKillerClient
TEMPLATE = app
CONFIG += c++14


SOURCES += main.cpp						\
        mainwindow.cpp 					\
        vkkiller_client.cpp 			\
        connection_to_host_dialog.cpp 	\
        changing_username_dialog.cpp \
    textedit.cpp \
    creating_topic_dialog.cpp

HEADERS  += mainwindow.h 				\
        vkkiller_client.h 				\
        vkkiller_request_reply.h 		\
        connection_to_host_dialog.h 	\
        vkkiller_server_constants.h 	\
        changing_username_dialog.h \
    textedit.h \
    creating_topic_dialog.h

FORMS    += mainwindow.ui 				\
    connection_to_host_dialog.ui 		\
    changing_username_dialog.ui \
    creating_topic_dialog.ui

RESOURCES += \
    resources.qrc \
    resources.qrc
