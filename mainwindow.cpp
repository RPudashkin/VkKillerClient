#include <QMessageBox>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "create_topic_dialog.h"
#include "connection_to_host_dialog.h"
#include "vkkiller_request_reply.h"

#include <QHostAddress>
#include <iostream>


MainWindow::MainWindow(QWidget* parent):
    QMainWindow		(parent),
    m_client		(std::make_unique<VkKillerClient>()),
    ui				(new Ui::MainWindow)

{
    ui->setupUi(this);

    connect(m_client.get(), &VkKillerClient::readyRead,
            this, 			&MainWindow::processReplyFromServer);

    connect(ui->connectToServerAction, &QAction::triggered,
            this,					   &MainWindow::openConnectionDialog);

    connect(m_client.get(), SIGNAL(error    (QAbstractSocket::SocketError)),
            this,         	SLOT  (slotError(QAbstractSocket::SocketError)));

    m_client->getTopicsListRequest();
}


MainWindow::~MainWindow() {
    m_client->disconnectFromHost();
    delete ui;
}


void MainWindow::openConnectionDialog() {
    ConnectionToHostDialog dialog;
    int result = dialog.exec();

    if (result != QDialog::Accepted)
        return;
    QHostAddress address = dialog.address();
    quint16       port   = dialog.port();

    m_client->connectToHost(address, port);
}


void MainWindow::connectionError(QAbstractSocket::SocketError socketError) {
    QString strError =  (socketError == QAbstractSocket::HostNotFoundError ?
                        "The host was not found." :
                         socketError == QAbstractSocket::RemoteHostClosedError ?
                         "The remote host is closed." :
                         socketError == QAbstractSocket::ConnectionRefusedError ?
                         "The connection was refused." :
                         QString(m_client->errorString()));
    QMessageBox::warning(0, "Connection error", strError);
}


void MainWindow::on_createTopic_clicked() {
    CreateTopicDialog dialog;
    int result = dialog.exec();

    if (result != QDialog::Accepted)
        return;

    QString topicName = dialog.topicName();
    QString message   = dialog.message();

    m_client->createTopicRequest  (topicName, message);
    m_client->getTopicsListRequest();
}


void MainWindow::on_topicsList_doubleClicked(const QModelIndex& index) {
    quint16 topicId = 0;
    m_client->getTopicHistoryRequest(topicId);
    ui->send->setEnabled(true);
}


void MainWindow::on_send_clicked() {
    QString msg 	= ui->messageLine->toPlainText().trimmed();
    quint16 topicId = 0;

    ui->messageLine->clear();
    ui->messageLine->setReadOnly(true);
    ui->send->setEnabled(false);
    ui->warningsLabel->setText("<font color='gray'><b>"
                               "Sending a message will be available in 30 seconds"
                               "</b></font>");

    QTimer::singleShot(30000, [this]() {
        ui->warningsLabel->clear();
        ui->messageLine->setReadOnly(false);
        ui->send->setEnabled(true);
    });

    m_client->sendTextMessageRequest(topicId, msg);
}


void MainWindow::processReplyFromServer() {
    QDataStream in(m_client.get());
    quint16 	blockSize = 0;

    in.setVersion(QDataStream::Qt_DefaultCompiledVersion);

    while (true) {
        if (!blockSize) {
            if (m_client->bytesAvailable() < sizeof(quint16))
                break;
            in >> blockSize;
        }

        if (m_client->bytesAvailable() < blockSize)
            break;

        blockSize = 0;

        quint8 reply;
        in >> reply;

        if (reply == Reply_type::OK) {
            QString msg;
            in >> msg;
            if (msg.isEmpty()) continue;

            quint8 currRequest = m_client->currRequest();

            if (currRequest == Request_type::GET_TOPICS_LIST) {
                updateTopicsList(msg);
            }
            else if (currRequest == Request_type::GET_TOPIC_HISTORY ||
                     currRequest == Request_type::GET_LAST_MESSAGES_FROM_TOPIC)
            {
                //updateTopicHistory(msg);
            }
        } // OK
        else if (reply == Reply_type::TOO_FAST_MESSAGING) {

        } // TOO_FAST_MESSAGING
        else if (reply == Reply_type::UNKNOWN_TOPIC) {

        } // UNKNOWN_TOPIC
        else if (reply == Reply_type::WRONG_NAME) {

        } // WRONG_NAME
        else if (reply == Reply_type::WRONG_MESSAGE) {

        } // WRONG_MESSAGE
        else if (reply == Reply_type::WRONG_TOPIC_NAME) {

        } // WRONG_TOPIC_NAME
        else if (reply == Reply_type::FAILED_TOPIC_CREATE) {

        } // FAILED_TOPIC_CREATE
        else {

        } // UNKNOWN_REQUEST
    }
}


void MainWindow::updateTopicsList(const QString& server_msg) noexcept {
    QStringList topicsList = server_msg.split(SEPARATING_CH);
    ui->topicsList->clear();

    QVector<int> 		topicsId;
    QVector<QString> 	topicsNames;
    QVector<int> 		topicsRatings;

    size_t finish = topicsList.size() - 3;
    for (size_t i = 0; i < finish; i += 3) {
        int     topicId 	= topicsList.at(i).toInt();
        QString topicName	= topicsList.at(i+1);
        int 	topicRating = topicsList.at(i+2).toInt();

        topicsId.push_back		(topicId);
        topicsNames.push_back	(std::move(topicName));
        topicsRatings.push_back	(topicRating);
    }

    // sort by rating here
    // ...

    for (auto& name: topicsNames)
        ui->topicsList->addItem(name);
}


void MainWindow::updateTopicHistory(const QString& server_msg, quint16 topicId) noexcept {
    QStringList topicsList = server_msg.split(SEPARATING_CH);
}