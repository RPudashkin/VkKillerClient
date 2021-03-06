﻿#include <QMessageBox>
#include <QFile>
#include <QStringBuilder>
#include <QSettings>

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "creating_topic_dialog.h"
#include "connection_to_host_dialog.h"
#include "changing_username_dialog.h"

#include "vkkiller_request_reply.h"
#include "vkkiller_server_constants.h"


MainWindow::MainWindow(QWidget* parent):
    QMainWindow           (parent),
    m_client              (std::make_unique<VkKillerClient>()),
    m_settings            (std::make_unique<QSettings>("VkKillers", "VkKillerClient")),
    ui                    (new Ui::MainWindow),
    m_blockedMessaging    (false),
    m_blockedTopicCreation(false)

{
    ui->setupUi(this);

    connect(m_client.get(), SIGNAL(error          (QAbstractSocket::SocketError)),
            this,           SLOT  (connectionError(QAbstractSocket::SocketError)));

    connect(m_client.get(),            &VkKillerClient::readyRead,
            this,                      &MainWindow::processReplyFromServer);

    connect(&m_updateCooldownTimer,    &QTimer::timeout,
            this,                      &MainWindow::updateCooldownTime);

    connect(ui->connectToServerAction, &QAction::triggered,
            this,                      &MainWindow::openConnectionDialog); 

    connect(ui->changeUsernameAction,  &QAction::triggered,
            this,                      &MainWindow::openUsernameChangeDialog);

    connect(ui->messageLine,           &TextEdit::pressedEnter,
            this,                      &MainWindow::on_send_clicked);

    restoreSettings();
}


MainWindow::~MainWindow() {
    m_client->disconnectFromHost();
    delete ui;
}


void MainWindow::restoreSettings() noexcept {
    ui->changeUsernameAction->setEnabled(false);
    ui->connectionStatusLabel->setText("[<font color = 'red'><b>Offline</b></font>]");

    m_address  = QHostAddress(m_settings->value("address").toString());
    m_port     = m_settings->value("port").toInt();
    m_username = m_settings->value("username").toString();

    if (!m_address.isNull() && m_port > 0)
        m_client->connectToHost(m_address, m_port);

    QMainWindow::restoreState   (m_settings->value("mainWindowState").toByteArray());
    QMainWindow::restoreGeometry(m_settings->value("mainWindowGeometry").toByteArray());
}


void MainWindow::openConnectionDialog() {
    ConnectionToHostDialog dialog;
    int result = dialog.exec();

    if (result != QDialog::Accepted)
        return;

    m_address = dialog.address();
    m_port    = dialog.port	  ();

    m_client->connectToHost(m_address, m_port);

    m_settings->setValue("address", m_address.toString());
    m_settings->setValue("port",    m_port);
}


void MainWindow::markClientAsOnline() noexcept {
    if (m_client->isValid()) {
        if (m_username != "anonymous" && !m_username.isEmpty())
            m_client->setNameRequest(m_username);

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

    m_username = dialog.username();
    m_client->setNameRequest(m_username);
    m_settings->setValue("username", m_username);
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
    ui->send->setEnabled                (false);
    ui->createTopic->setEnabled         (false);
    ui->changeUsernameAction->setEnabled(false);
}


void MainWindow::on_createTopic_clicked() {
    if (m_blockedTopicCreation) {
        QMessageBox::warning(0, "Warning", "You may create only one topic per 3 minutes!");
        return;
    }

    CreatingTopicDialog dialog;
    int result = dialog.exec();

    if (result != QDialog::Accepted)
        return;

    QString topicName = dialog.topicName();
    QString message   = dialog.message  ();
    m_client->createTopicRequest(topicName, message);

    m_blockedTopicCreation = true;
    QTimer::singleShot(Server_constant::TOPIC_CREATING_COOLDOWN * 1000, [this]() {
        m_blockedTopicCreation = false;
    });
}


void MainWindow::blockMessaging() noexcept {
    ui->messageLine->clear       ();
    ui->send->setEnabled         (false);
    ui->warningsLabel->setText   ("<font color='gray'><b>"
                                  "Sending a message will be available in 15 sec."
                                  "</b></font>");
    m_blockedMessaging  = true;
    m_startCooldownTime = QTime::currentTime();
    m_updateCooldownTimer.start(1000);
}


void MainWindow::on_topicsList_clicked(QModelIndex index) {
    ui->topicHistory->clear();
    int row = index.row();

    if (m_currTopicId == m_topicsId[row]) {
        m_client->getLastMessagesRequest(m_currTopicId);
        return;
    }
    else {
        m_currTopicId = m_topicsId[row];
        m_client->getTopicHistoryRequest(m_currTopicId);
    }

    if (!m_blockedMessaging) {
        ui->messageLine->setReadOnly(false);
        ui->send->setEnabled        (true);
    }

    ui->update->setEnabled(true);
}


void MainWindow::on_update_clicked() {
    if (m_client->isWritable())
        m_client->getTopicsListRequest();
}


void MainWindow::on_send_clicked() {
    QString msg = ui->messageLine->toPlainText().trimmed();
    if (msg.isEmpty() || !ui->send->isEnabled()) return;

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

    ui->warningsLabel->setText("<font color='gray'><b>"
                               "Sending a message will be available in "
                               + QString::number(cooldown) +
                               " sec. </b></font>");
    if (!cooldown) {
        ui->warningsLabel->clear    ();
        ui->send->setEnabled        (true);
        ui->createTopic->setEnabled (true);
        m_blockedMessaging         = false;
        m_updateCooldownTimer.stop  ();
    }
}


void MainWindow::processReplyFromServer() {
    QDataStream in(m_client.get());
    quint16     blockSize = 0;

    in.setVersion(QDataStream::Qt_DefaultCompiledVersion);

    auto warning = [this](const QString& str) {
        ui->warningsLabel->setText("<font color=#FF8C00><b>" % str % "</b></font>");
    };

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

        if (reply == Reply_type::CONNECTED) {
            markClientAsOnline();
            continue;
        }
        else if (reply == Reply_type::TOPICS_LIST) {
            QString msg;
            in >> msg;

            updateTopicsList(msg);
        }
        else if (reply == Reply_type::TOPIC_HISTORY ||
                 reply == Reply_type::LAST_MESSAGES)
        {
            QString msg;
            in >> msg;

            updateTopicHistory(msg);
        }

        else if (reply == Reply_type::TOO_FAST_MESSAGING)
            warning("Too fast messaging!");

        else if (reply == Reply_type::TOO_FAST_TOPIC_CREATING)
            warning("Too fast topic creating!");

        else if (reply == Reply_type::UNKNOWN_TOPIC)
            warning("Topic does not exist!");

        else if (reply == Reply_type::WRONG_NAME)
            warning("Server has rejected username!");

        else if (reply == Reply_type::WRONG_MESSAGE)
            warning("Server has rejected message!");

        else if (reply == Reply_type::WRONG_TOPIC_NAME)
            warning("Server has rejected topic name!");

        else if (reply == Reply_type::FAILED_TOPIC_CREATE)
            warning("Failed topic create!");

        else warning("Unknown request!");
    }
}


void MainWindow::updateTopicsList(const QString& server_msg) noexcept {
    if (server_msg.isEmpty())
        return;
    if (!ui->warningsLabel->text().isEmpty() && !m_blockedMessaging)
        ui->warningsLabel->clear();

    QStringList topicsList = server_msg.split(Server_constant::SEPARATING_CH);
    ui->topicsList->clear();
    m_topicsId.clear();

    // id-topicName-rating
    using entry_t = std::tuple<int, QString, int>;
    QVector<entry_t> sorted;

    size_t finish = topicsList.size() - 3;
    for (size_t i = 0; i < finish; i += 3) {
        int     topicId     = topicsList.at(i).toInt();
        QString topicName   = topicsList.at(i+1);
        int     topicRating = topicsList.at(i+2).toInt();

        sorted.push_back(std::make_tuple(topicId, std::move(topicName), topicRating));
    }

    std::sort(sorted.begin(), sorted.end(),
    [](const entry_t& entry1, const entry_t& entry2) {
        return std::get<2>(entry1) > std::get<2>(entry2);
    });

    for (entry_t& entry: sorted) {
        m_topicsId.push_back    (std::get<0>(entry));
        ui->topicsList->addItem (std::get<1>(entry));

        int num = m_topicsId.size() - 1;
        if (m_topicsId[num] == m_currTopicId)
            ui->topicsList->setCurrentRow(num);
    }
}


void MainWindow::updateTopicHistory(const QString& server_msg) noexcept {
    if (server_msg.isEmpty())
        return;
    if (!ui->warningsLabel->text().isEmpty() && !m_blockedMessaging)
        ui->warningsLabel->clear();

    QStringList history = server_msg.split(Server_constant::SEPARATING_CH);

    auto colorGen = [](size_t authorId) {
        size_t  code  = (authorId + 1123) * 98765 % 16000000;
        QString color = QString("%1").arg(code, 0, 16);
        int     len   = color.length();

        if (len < 6) {
            for (int i = 6 - len; i > 0; i--)
                color = '0' % color;
        }

        return '#' + color;
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

        QString entry = "<font color="
                      % color
                      % "><b>"
                      % author
                      % "</b></font> <i>#"
                      % authorId
                      % "</i> ["
                      % time
                      % "] ["
                      % date
                      % "]<p>"
                      % message
                      % "</p>";
        ui->topicHistory->append(entry);
    }
}


void MainWindow::closeEvent(QCloseEvent* event) {
    m_settings->setValue("address",            m_address.toString());
    m_settings->setValue("port",               m_port);
    m_settings->setValue("username",           m_username);
    m_settings->setValue("mainWindowState",    saveState());
    m_settings->setValue("mainWindowGeometry", saveGeometry());
    QMainWindow::close();
}