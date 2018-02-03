#ifndef CONNECTION_TO_HOST_DIALOG_H
#define CONNECTION_TO_HOST_DIALOG_H

#include <QDialog>
#include <QHostAddress>

namespace Ui {
    class ConnectionToHostDialog;
}


class ConnectionToHostDialog: public QDialog {
    Q_OBJECT

public:
    explicit ConnectionToHostDialog(QWidget* parent = nullptr);
    ~ConnectionToHostDialog();

    QHostAddress address() const noexcept;
    quint16		 port   () const noexcept;

private slots:
    void on_connect_clicked();

private:
    Ui::ConnectionToHostDialog* ui;
    QHostAddress				m_address;
    quint16						m_port;
};

#endif // CONNECTION_TO_HOST_DIALOG_H