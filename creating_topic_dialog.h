#ifndef CREATING_TOPIC_DIALOG_H
#define CREATING_TOPIC_DIALOG_H

#include <QDialog>

namespace Ui {
    class CreatingTopicDialog;
}


class CreatingTopicDialog: public QDialog {
    Q_OBJECT

public:
    explicit CreatingTopicDialog(QWidget* parent = nullptr);
    ~CreatingTopicDialog();

    QString topicName() const noexcept;
    QString message  () const noexcept;

private slots:
    void on_ok_clicked();

    void on_messageLine_textChanged();

private:
    Ui::CreatingTopicDialog* ui;
    QString                  m_topicName;
    QString                  m_message;
};

#endif // CREATING_TOPIC_DIALOG_H