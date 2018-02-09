#ifndef CHANGING_USERNAME_DIALOG_H
#define CHANGING_USERNAME_DIALOG_H

#include <QDialog>

namespace Ui {
    class ChangingUsernameDialog;
}


class ChangingUsernameDialog: public QDialog {
    Q_OBJECT

public:
    explicit ChangingUsernameDialog(QWidget* parent = nullptr);
    ~ChangingUsernameDialog();

    QString username() const noexcept;

private slots:
    void on_usernameLine_textChanged(const QString &arg1);
    void on_ok_clicked();

private:
    Ui::ChangingUsernameDialog* ui;
    QString                     m_username;
};

#endif // CHANGING_USERNAME_DIALOG_H
