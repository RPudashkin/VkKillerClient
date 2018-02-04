#include <QMessageBox>
#include "create_topic_dialog.h"
#include "ui_create_topic_dialog.h"
#include "vkkiller_server_constants.h"


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
    QString topicName	= ui->nameLine->text().trimmed();
    QString message     = ui->messageLine->toPlainText().trimmed();

    if (topicName.isEmpty())
        QMessageBox::warning(0, "Creation topic error", "Enter the topic name!");
    else if (message.isEmpty())
        QMessageBox::warning(0, "Creation topic error", "Enter the message!");
    else {
        m_topicName = std::move(topicName);
        m_message   = std::move(message);
        accept();
    }
}


void CreateTopicDialog::on_messageLine_textChanged() {
    QString text = ui->messageLine->toPlainText();

    if (text.length() > Server_constant::MAX_MESSAGE_LENGTH) {
        ui->messageLine->setText(text.left(Server_constant::MAX_MESSAGE_LENGTH));
        ui->messageLine->moveCursor(QTextCursor::EndOfBlock);
    }
}


QString CreateTopicDialog::topicName() const noexcept {
    return m_topicName;
}


QString CreateTopicDialog::message() const noexcept {
    return m_message;
}