#include <QMessageBox>
#include "create_topic_dialog.h"
#include "ui_create_topic_dialog.h"


CreateTopicDialog::CreateTopicDialog(QWidget* parent) :
    QDialog     (parent),
    ui          (new Ui::CreateTopicDialog),
    m_topicName (""),
    m_message   ("")
{
    ui->setupUi(this);
    setFixedSize(600, 300);
}

CreateTopicDialog::~CreateTopicDialog() {
    delete ui;
}


void CreateTopicDialog::on_ok_clicked() {
    QString topicName    = ui->nameLine->text().trimmed();
    QString message      = ui->messageLine->toPlainText().trimmed();
    size_t  topicNameLen = topicName.length();
    size_t  messageLen   = message.length();

    constexpr size_t MAX_TOPIC_NAME_LEN = 150;
    constexpr size_t MAX_MESSAGE_LEN    = 300;

    if (topicName.isEmpty())
        QMessageBox::warning(0, "Creation topic error", "Enter the topic name!");
    else if (message.isEmpty())
        QMessageBox::warning(0, "Creation topic error", "Enter the message!");
    else if (topicNameLen > MAX_TOPIC_NAME_LEN)
        QMessageBox::warning(0, "Creation topic error", "The topic name is too long!");
    else if (messageLen > MAX_MESSAGE_LEN)
        QMessageBox::warning(0, "Creation topic error", "The message is too long!");
    else {
        m_topicName = std::move(topicName);
        m_message   = std::move(message);
        accept();
    }
}


QString CreateTopicDialog::topicName() const noexcept {
    return m_topicName;
}


QString CreateTopicDialog::message() const noexcept {
    return m_message;
}
