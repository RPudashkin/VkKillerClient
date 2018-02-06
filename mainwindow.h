#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QHostAddress>
#include <QTimer>
#include <QTime>
#include <QFile>
#include <memory>
#include "vkkiller_client.h"

namespace Ui {
    class MainWindow;
}



class MainWindow: public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    void on_topicsList_currentRowChanged    (int currentRow);
    void on_updateTopicHistoryButton_clicked();
    void on_updateTopicsListButton_clicked  ();
    void on_send_clicked                    ();
    void on_createTopic_clicked             ();
    void on_messageLine_textChanged         ();

    void openConnectionDialog               ();
    void openUsernameChangeDialog           ();

    void connectionError                    (QAbstractSocket::SocketError err);
    void processReplyFromServer             ();
    void updateCooldownTime                 ();

private:
    void loadConfig         ()                                          noexcept;
    void connectToHost      (const QHostAddress& address, quint16 port) noexcept;
    void updateTopicsList   (const QString& server_msg)                 noexcept;
    void updateTopicHistory (const QString& server_msg)                 noexcept;
    void blockMessaging     ()                                          noexcept;

    std::unique_ptr<VkKillerClient> m_client;
    std::unique_ptr<QFile>          m_config;
    QHostAddress                    m_address;
    quint16                         m_port;
    QVector<quint16>                m_topicsId;
    Ui::MainWindow*                 ui;
    QTimer                          m_updateCooldownTimer;
    QTime                           m_startCooldownTime;
    bool                            m_blockedMessaging;
    bool                            m_blockedTopicCreation;
};

#endif // MAINWINDOW_H
