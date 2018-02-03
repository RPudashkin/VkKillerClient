#ifndef CREATE_TOPIC_DIALOG_H
#define CREATE_TOPIC_DIALOG_H

#include <QDialog>

namespace Ui {
    class CreateTopicDialog;
}


class CreateTopicDialog: public QDialog {
    Q_OBJECT

public:
    explicit CreateTopicDialog(QWidget* parent = nullptr);
    ~CreateTopicDialog();

    QString topicName() const noexcept;
    QString message  () const noexcept;

private slots:
    void on_ok_clicked();

private:
    Ui::CreateTopicDialog* ui;
    QString                m_topicName;
    QString                m_message;
};

#endif // CREATE_TOPIC_DIALOG_H
