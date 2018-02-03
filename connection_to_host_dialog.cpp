#include <QMessageBox>
#include "connection_to_host_dialog.h"
#include "ui_connection_to_host_dialog.h"


ConnectionToHostDialog::ConnectionToHostDialog(QWidget* parent):
    QDialog (parent),
    ui      (new Ui::ConnectionToHostDialog)
{
    ui->setupUi(this);
    setFixedSize(400, 200);
}


ConnectionToHostDialog::~ConnectionToHostDialog() {
    delete ui;
}


void ConnectionToHostDialog::on_connect_clicked() {
    QString address = ui->addressLine->text();
    quint16 port    = ui->portLine->value();

    if (address.isEmpty())
        QMessageBox::warning(0, "Connection error", "Enter the ip address!");
    else {
        m_address = QHostAddress(std::move(address));
        m_port    = port;
        accept();
    }
}


QHostAddress ConnectionToHostDialog::address() const noexcept {
    return m_address;
}


quint16 ConnectionToHostDialog::port() const noexcept {
    return m_port;
}
