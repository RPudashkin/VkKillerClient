#include <QMessageBox>
#include <QFile>
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "create_topic_dialog.h"
#include "connection_to_host_dialog.h"
#include "changing_username_dialog.h"

#include "vkkiller_request_reply.h"
#include "vkkiller_server_constants.h"

#include <QToolBar>


MainWindow::MainWindow(QWidget* parent):
    QMainWindow           (parent),
    m_client              (std::make_unique<VkKillerClient>()),
    m_config              (std::make_unique<QFile>("vkkiller_client.config")),
    ui                    (new Ui::MainWindow),
    m_blockedMessaging    (false),
    m_blockedTopicCreation(false)

{
    ui->setupUi(this);

    connect(m_client.get(), &VkKillerClient::readyRead,
            this,           &MainWindow::processReplyFromServer);

    connect(m_client.get(), SIGNAL(error          (QAbstractSocket::SocketError)),
            this,           SLOT  (connectionError(QAbstractSocket::SocketError)));

    connect(ui->connectToServerAction, &QAction::triggered,
            this,                      &MainWindow::openConnectionDialog); 

    connect(ui->changeUsernameAction,  &QAction::triggered,
            this,                      &MainWindow::openUsernameChangeDialog);

    connect(&m_updateCooldownTimer, SIGNAL(timeout()),
            this,                   SLOT  (updateCooldownTime()));

    loadConfig();
}


MainWindow::~MainWindow() {
    m_client->disconnectFromHost();
    if (m_config->isOpen()) m_config->close();
    delete ui;
}


void MainWindow::loadConfig() noexcept {
    ui->changeUsernameAction->setEnabled(false);
    ui->connectionStatusLabel->setText("[<font color = 'red'><b>Offline</b></font>]");

    if (m_config->exists() && m_config->open(QIODevice::ReadWrite)) {
        QVector<QString> buffer;

        while (!m_config->atEnd())
            buffer.push_back(QString(m_config->readLine()));

        if (buffer.size() == 3) {
            if (buffer[0].length() != 0 && buffer[1].length() != 0 && buffer[2].length() != 0) {
                QString username  = QString(buffer[2]);
                        m_address = QHostAddress(buffer[0]);
                        m_port    = QString(buffer[1]).toInt();

                connectToHost(m_address, m_port);

                if (username != "anonymous")
                    m_client->setNameRequest(username);
            }
        }
        m_client->getTopicsListRequest();
    } // open config

    m_config->close();
}


void MainWindow::openConnectionDialog() {
    ConnectionToHostDialog dialog;
    int result = dialog.exec();

    if (result != QDialog::Accepted)
        return;

    m_address = dialog.address();
    m_port    = dialog.port	  ();

    connectToHost(m_address, m_port);

    m_config->open(QIODevice::WriteOnly);
    QTextStream out(m_config.get());
    out << m_address.toString() << "\n" << QString::number(m_port) << "\n" << "anonymous";
    m_config->close();
}


void MainWindow::connectToHost(const QHostAddress &address, quint16 port) noexcept {
    m_client->connectToHost(address, port);

    if (m_client->isValid()) {
        m_client->getTopicsListRequest      ();
        ui->warningsLabel->clear            ();
        ui->createTopic->setEnabled         (true);
        ui->changeUsernameAction->setEnabled(true);
        ui->connectionStatusLabel->setText  ("[<font color = 'green'><b>Online</b></font>]");
    }
}


void MainWindow::openUsernameChangeDialog() {
    ChangingUsernameDialog dialog;
    int result = dialog.exec();

    if (result != QDialog::Accepted)
        return;

    QString username = dialog.username();
    m_client->setNameRequest(username);

    m_config->open(QIODevice::WriteOnly);
    QTextStream out(m_config.get());
    out << m_address.toString() << "\n" << QString::number(m_port) << "\n" << username;
    m_config->close();
}


void MainWindow::connectionError(QAbstractSocket::SocketError socketError) {
    QString strError =  "<font color = 'red'><b>Error: " +
                        (socketError == QAbstractSocket::HostNotFoundError      ?
                        "the host was not found" :
                         socketError == QAbstractSocket::RemoteHostClosedError  ?
                         "the remote host is closed" :
                         socketError == QAbstractSocket::ConnectionRefusedError ?
                         "the connection was refused" :
                         QString(m_client->errorString())) +
                        "</b></font>";

    ui->connectionStatusLabel->setText  ("[<font color = 'red'><b>Offline</b></font>]");
    ui->warningsLabel->setText          (strError);
    ui->messageLine->clear              ();
    ui->messageLine->setReadOnly        (true);
    ui->send->setEnabled                (false);
    ui->createTopic->setEnabled         (false);
    ui->changeUsernameAction->setEnabled(false);
}


void MainWindow::on_createTopic_clicked() {
    if (m_blockedTopicCreation) {
        QMessageBox::warning(0, "Warning", "You may create only one topic per 3 minutes!");
        return;
    }

    CreateTopicDialog dialog;
    int result = dialog.exec();

    if (result != QDialog::Accepted)
        return;

    QString topicName = dialog.topicName();
    QString message   = dialog.message  ();
    m_client->createTopicRequest        (topicName, message);
    m_client->getTopicsListRequest      ();

    m_blockedTopicCreation = true;
    QTimer::singleShot(Server_constant::TOPIC_CREATING_COOLDOWN * 1000, [this]() {
        m_blockedTopicCreation = false;
    });
}


void MainWindow::blockMessaging() noexcept {
    ui->messageLine->clear      ();
    ui->messageLine->setReadOnly(true);
    ui->send->setEnabled        (false);
    ui->warningsLabel->setText  ("<font color='gray'><b>"
                                 "Sending a message will be available in 15 sec."
                                 "</b></font>");
    m_blockedMessaging  = true;
    m_startCooldownTime = QTime::currentTime();
    m_updateCooldownTimer.start(1000);
}


void MainWindow::on_topicsList_currentRowChanged(int currentRow) {
    ui->topicHistory->clear();
    if (currentRow < 0) return;

    quint16 topicId = m_topicsId[currentRow];
    m_client->getTopicHistoryRequest(topicId);

    if (!m_blockedMessaging) {
        ui->messageLine->setReadOnly(false);
        ui->send->setEnabled        (true);
    }
}


void MainWindow::on_updateTopicHistoryButton_clicked() {
    if (m_client->isWritable()) {
        int     row     = ui->topicsList->currentRow();
        quint16 topicId = m_topicsId[row];

        m_client->getLastMessagesRequest(topicId);
    }

}


void MainWindow::on_updateTopicsListButton_clicked() {
    if (m_client->isWritable())
        m_client->getTopicsListRequest();
}



void MainWindow::on_send_clicked() {
    QString msg = ui->messageLine->toPlainText().trimmed();
    if (msg.isEmpty()) return;

    int     row     = ui->topicsList->currentRow();
    quint16 topicId = m_topicsId[row];

    m_client->sendTextMessageRequest(topicId, msg);
    m_client->getLastMessagesRequest(topicId);
    blockMessaging();
}


void MainWindow::on_messageLine_textChanged() {
    QString text = ui->messageLine->toPlainText();

    if (text.length() > Server_constant::MAX_MESSAGE_LENGTH) {
        ui->messageLine->setText(text.left(Server_constant::MAX_MESSAGE_LENGTH));
        ui->messageLine->moveCursor(QTextCursor::EndOfBlock);
    }
}


void MainWindow::updateCooldownTime() {
    QTime currTime = QTime::currentTime();
    int   secs     = m_startCooldownTime.secsTo(currTime);

    int cooldown = Server_constant::MESSAGING_COOLDOWN - secs;
    if (cooldown < 0) cooldown = 0;

    ui->warningsLabel->setText  ("<font color='gray'><b>"
                                 "Sending a message will be available in "
                                 + QString::number(cooldown) +
                                 " sec. </b></font>");
    if (!cooldown) {
        ui->warningsLabel->clear    ();
        ui->messageLine->setReadOnly(false);
        ui->send->setEnabled        (true);
        ui->createTopic->setEnabled (true);
        m_blockedMessaging =        false;
        m_updateCooldownTimer.stop  ();
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
            ui->warningsLabel->setText("<font color=#FF8C00><b>"
                                       "Too fast messaging!"
                                       "</b></font>");
        } // TOO_FAST_MESSAGING
        else if (reply == Reply_type::TOO_FAST_TOPIC_CREATING) {
            ui->warningsLabel->setText("<font color=#FF8C00><b>"
                                       "Too fast topic creating!"
                                       "</b></font>");
        }
        else if (reply == Reply_type::UNKNOWN_TOPIC) {
            ui->warningsLabel->setText("<font color=#FF8C00><b>"
                                       "Topic does not exist!"
                                       "</b></font>");
        } // UNKNOWN_TOPIC
        else if (reply == Reply_type::WRONG_NAME) {
            ui->warningsLabel->setText("<font color=#FF8C00><b>"
                                       "Server has rejected username!"
                                       "</b></font>");
        } // WRONG_NAME
        else if (reply == Reply_type::WRONG_MESSAGE) {
            ui->warningsLabel->setText("<font color=#FF8C00><b>"
                                       "Server has rejected message!"
                                       "</b></font>");
        } // WRONG_MESSAGE
        else if (reply == Reply_type::WRONG_TOPIC_NAME) {
            ui->warningsLabel->setText("<font color=#FF8C00><b>"
                                       "Server has rejected topic name!"
                                       "</b></font>");
        } // WRONG_TOPIC_NAME
        else if (reply == Reply_type::FAILED_TOPIC_CREATE) {
            ui->warningsLabel->setText("<font color=#FF8C00><b>"
                                       "Failed topic create!"
                                       "</b></font>");
        } // FAILED_TOPIC_CREATE
        else {
            ui->warningsLabel->setText("<font color=#FF8C00><b>"
                                       "Unknown request!"
                                       "</b></font>");
        } // UNKNOWN_REQUEST
    }
}


void MainWindow::updateTopicsList(const QString& server_msg) noexcept {
    QStringList topicsList = server_msg.split(Server_constant::SEPARATING_CH);
    ui->topicsList->clear();
    m_topicsId.clear();

    QVector<QString>    topicsNames;
    QVector<int>        topicsRatings;

    size_t finish = topicsList.size() - 3;
    for (size_t i = 0; i < finish; i += 3) {
        int     topicId     = topicsList.at(i).toInt();
        QString topicName   = topicsList.at(i+1);
        int     topicRating = topicsList.at(i+2).toInt();

        m_topicsId.push_back   (topicId);
        topicsNames.push_back  (std::move(topicName));
        topicsRatings.push_back(topicRating);
    }

    // sort by rating here
    // ...

    for (auto& name: topicsNames)
        ui->topicsList->addItem(name);
}


void MainWindow::updateTopicHistory(const QString& server_msg) noexcept {
    QStringList history = server_msg.split(Server_constant::SEPARATING_CH);

    auto colorGen = [](size_t authorId) {
        size_t  code  = (authorId + 1123) * 98765 % 16000000;
        QString color = QString("%1").arg(code, 0, 16);
        int     len   = color.length();

        if (len < 6) {
            for (int i = 6 - len; i > 0; i--)
                color = "0" + color;
        }

        return "#" + color;
    };

    size_t finish = history.size() - 4;
    for (size_t i = 0; i < finish; i += 5) {
        QString author   = history.at(i);
        QString authorId = history.at(i+1);
        QString time     = history.at(i+2);
        QString date     = history.at(i+3);
        QString message  = history.at(i+4);

        QString color = "'gray'";
        if (author != "anonymous")
            color = colorGen(authorId.toInt());

        QString entry    = "<font color="
                         + color
                         + "><b>"
                         + author
                         + "</b></font> <i>#"
                         + authorId
                         + "</i> ["
                         + time
                         + "] ["
                         + date
                         + "]<p>"
                         + message
                         + "</p>";
        ui->topicHistory->append(entry);
    }
}