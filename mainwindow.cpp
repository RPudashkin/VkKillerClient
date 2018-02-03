#include <QMessageBox>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "create_topic_dialog.h"
#include "connection_to_host_dialog.h"
#include "vkkiller_request_reply.h"

#include <QTextEdit>
#include <iostream>


MainWindow::MainWindow(QWidget* parent):
    QMainWindow     	(parent),
    m_client        	(std::make_unique<VkKillerClient>()),
    ui              	(new Ui::MainWindow),
    m_blockedMessaging	(false)

{
    ui->setupUi(this);

    connect(m_client.get(), &VkKillerClient::readyRead,
            this,           &MainWindow::processReplyFromServer);

    connect(ui->connectToServerAction, &QAction::triggered,
            this,                      &MainWindow::openConnectionDialog);

    connect(m_client.get(), SIGNAL(error    (QAbstractSocket::SocketError)),
            this,           SLOT  (slotError(QAbstractSocket::SocketError)));

    connect(&m_updateCooldownTimer, SIGNAL(timeout()),
            this, 				    SLOT  (updateCooldownTime()));
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
    m_client->getTopicsListRequest();
    ui->createTopic->setEnabled(true);
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
    QString message   = dialog.message  ();
    m_client->createTopicRequest  		(topicName, message);
    m_client->getTopicsListRequest		();
    blockMessaging				  		();
}


void MainWindow::on_topicsList_currentRowChanged(int currentRow) {
    ui->topicHistory->clear();
    if (currentRow < 0) return;

    quint16 topicId = m_topicsId[currentRow];
    m_client->getTopicHistoryRequest(topicId);

    if (!m_blockedMessaging)
        ui->send->setEnabled(true);
}


void MainWindow::on_send_clicked() {
    QString msg 	= ui->messageLine->toPlainText().trimmed();
    int		row		= ui->topicsList->currentRow();
    quint16 topicId = m_topicsId[row];

    std::cout << topicId << std::endl;

    m_client->sendTextMessageRequest(topicId, msg);
    m_client->getLastMessagesRequest(topicId);
    blockMessaging();
}


void MainWindow::blockMessaging() noexcept {
    ui->messageLine->clear		();
    ui->messageLine->setReadOnly(true);
    ui->send->setEnabled		(false);
    ui->createTopic->setEnabled	(false);
    ui->warningsLabel->setText	("<font color='gray'><b>"
                                 "Sending a message will be available in 15 sec."
                                 "</b></font>");
    m_blockedMessaging  = true;
    m_startCooldownTime = QTime::currentTime();
    m_updateCooldownTimer.start(1000);
}


void MainWindow::updateCooldownTime() {
    QTime currTime = QTime::currentTime();
    int secs = m_startCooldownTime.secsTo(currTime);

    int cooldown = MESSAGING_COOLDOWN - secs;
    if (cooldown < 0) cooldown = 0;

    ui->warningsLabel->setText	("<font color='gray'><b>"
                                 "Sending a message will be available in "
                                 + QString::number(cooldown) +
                                 " sec. </b></font>");
    if (!cooldown) {
        ui->warningsLabel->clear	();
        ui->messageLine->setReadOnly(false);
        ui->send->setEnabled		(true);
        ui->createTopic->setEnabled	(true);
        m_blockedMessaging         = false;
    }
}


void MainWindow::processReplyFromServer() {
    QDataStream in(m_client.get());
    quint16     blockSize = 0;

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
                updateTopicHistory(msg);
            }

            if (!ui->warningsLabel->text().isEmpty() && !m_blockedMessaging)
                ui->warningsLabel->clear();
        } // OK
        else if (reply == Reply_type::TOO_FAST_MESSAGING) {
            ui->warningsLabel->setText("<font color='red'><b>"
                                       "Too fast messaging!"
                                       "</b></font>");
        } // TOO_FAST_MESSAGING
        else if (reply == Reply_type::UNKNOWN_TOPIC) {
            ui->warningsLabel->setText("<font color='red'><b>"
                                       "Topic does not exist!"
                                       "</b></font>");
        } // UNKNOWN_TOPIC
        else if (reply == Reply_type::WRONG_NAME) {
            ui->warningsLabel->setText("<font color='red'><b>"
                                       "Server has rejected username!"
                                       "</b></font>");
        } // WRONG_NAME
        else if (reply == Reply_type::WRONG_MESSAGE) {
            ui->warningsLabel->setText("<font color='red'><b>"
                                       "Server has rejected message!"
                                       "</b></font>");
        } // WRONG_MESSAGE
        else if (reply == Reply_type::WRONG_TOPIC_NAME) {
            ui->warningsLabel->setText("<font color='red'><b>"
                                       "Server has rejected topic name!"
                                       "</b></font>");
        } // WRONG_TOPIC_NAME
        else if (reply == Reply_type::FAILED_TOPIC_CREATE) {
            ui->warningsLabel->setText("<font color='red'><b>"
                                       "Failed topic create!"
                                       "</b></font>");
        } // FAILED_TOPIC_CREATE
        else {
            ui->warningsLabel->setText("<font color='red'><b>"
                                       "Unknown request!"
                                       "</b></font>");
        } // UNKNOWN_REQUEST
    }
}


void MainWindow::updateTopicsList(const QString& server_msg) noexcept {
    QStringList topicsList = server_msg.split(SEPARATING_CH);
    ui->topicsList->clear();
    m_topicsId.clear();

    QVector<QString>	topicsNames;
    QVector<int>        topicsRatings;

    size_t finish = topicsList.size() - 3;
    for (size_t i = 0; i < finish; i += 3) {
        int     topicId     = topicsList.at(i).toInt();
        QString topicName   = topicsList.at(i+1);
        int     topicRating = topicsList.at(i+2).toInt();

        m_topicsId.push_back    (topicId);
        topicsNames.push_back   (std::move(topicName));
        topicsRatings.push_back (topicRating);
    }

    // sort by rating here
    // ...

    for (auto& name: topicsNames)
        ui->topicsList->addItem(name);
}


void MainWindow::updateTopicHistory(const QString& server_msg) noexcept {
    QStringList history = server_msg.split(SEPARATING_CH);

    size_t finish = history.size() - 4;
    for (size_t i = 0; i < finish; i += 5) {
        QString author   = history.at(i);
        size_t  authorId = history.at(i+1).toInt();
        QString time     = history.at(i+2);
        QString date     = history.at(i+3);
        QString message  = history.at(i+4);

        QString entry    = "<font color = 'gray'><b>"
                         + author
                         + "</b></font> "
                         + time
                         + " "
                         + date
                         + "<p>"
                         + message
                         + "</p>";
        ui->topicHistory->append(entry);
    }
}