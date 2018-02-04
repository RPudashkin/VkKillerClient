#include <QValidator>
#include "changing_username_dialog.h"
#include "ui_changing_username_dialog.h"


ChangingUsernameDialog::ChangingUsernameDialog(QWidget* parent):
    QDialog     (parent),
    ui          (new Ui::ChangingUsernameDialog),
    m_username  ("anonymous")
{
    ui->setupUi(this);
    setFixedSize(400, 100);
    
    QValidator* validator = new QRegExpValidator(QRegExp("[a-zA-Z0-9а-яёА-ЯЁ#_]*"), this);
    ui->usernameLine->setValidator(validator);
}


ChangingUsernameDialog::~ChangingUsernameDialog() {
    delete ui;
}


void ChangingUsernameDialog::on_usernameLine_textChanged(const QString& arg1) {
    if (!arg1.isEmpty())
        ui->ok->setEnabled(true);
    else
        ui->ok->setEnabled(false);
}


void ChangingUsernameDialog::on_ok_clicked() {
    m_username = ui->usernameLine->text();
    accept();
}


QString ChangingUsernameDialog::username() const noexcept {
    return m_username;
}
