﻿#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QTime>
#include <QFile>
#include <memory>
#include "vkkiller_client.h"

namespace Ui {
    class MainWindow;
}

class QHostAddress;


class MainWindow: public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    void on_topicsList_currentRowChanged(int currentRow);
    void on_send_clicked                ();
    void on_createTopic_clicked         ();
    void on_messageLine_textChanged     ();

    void openConnectionDialog           ();
    void openUsernameChangeDialog       ();

    void connectionError                (QAbstractSocket::SocketError err);
    void processReplyFromServer         ();
    void updateCooldownTime             ();

private:
    void connectToHost      (const QHostAddress& address, quint16 port) noexcept;
    void updateTopicsList   (const QString& server_msg)                 noexcept;
    void updateTopicHistory (const QString& server_msg)                 noexcept;
    void blockMessaging     ()                                          noexcept;

    std::unique_ptr<VkKillerClient> m_client;
    std::unique_ptr<QFile>          m_config;
    QVector<quint16>                m_topicsId;
    Ui::MainWindow*                 ui;
    QTimer                          m_updateCooldownTimer;
    QTime                           m_startCooldownTime;
    bool                            m_blockedMessaging;
};

#endif // MAINWINDOW_H
