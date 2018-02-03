#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
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
    void on_send_clicked                ();
    void on_topicsList_doubleClicked    (const QModelIndex& index);
    void on_createTopic_clicked         ();
    void openConnectionDialog           ();
    void connectionError                (QAbstractSocket::SocketError err);
    void processReplyFromServer         ();

private:
    static constexpr char SEPARATING_CH	= '\1';

    void updateTopicsList   (const QString& server_msg)                  noexcept;
    void updateTopicHistory (const QString& server_msg, quint16 topicId) noexcept;

    std::unique_ptr<VkKillerClient> m_client;
    Ui::MainWindow*                 ui;
    quint16                         m_selectedTopicNum;
    QTimer                          m_updateTopicsListTimer;
};

#endif // MAINWINDOW_H