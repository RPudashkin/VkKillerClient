#include <QMessageBox>
#include "creating_topic_dialog.h"
#include "ui_creating_topic_dialog.h"
#include "vkkiller_server_constants.h"


CreatingTopicDialog::CreatingTopicDialog(QWidget* parent) :
    QDialog     (parent),
    ui          (new Ui::CreatingTopicDialog),
    m_topicName (""),
    m_message   ("")
{
    ui->setupUi(this);
    setFixedSize(600, 300);
}

CreatingTopicDialog::~CreatingTopicDialog() {
    delete ui;
}


void CreatingTopicDialog::on_ok_clicked() {
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


void CreatingTopicDialog::on_messageLine_textChanged() {
    QString text = ui->messageLine->toPlainText();

    if (text.length() > Server_constant::MAX_MESSAGE_LENGTH) {
        ui->messageLine->setText(text.left(Server_constant::MAX_MESSAGE_LENGTH));
        ui->messageLine->moveCursor(QTextCursor::EndOfBlock);
    }
}


QString CreatingTopicDialog::topicName() const noexcept {
    return m_topicName;
}


QString CreatingTopicDialog::message() const noexcept {
    return m_message;
}