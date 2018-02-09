#ifndef TEXTEDIT_H
#define TEXTEDIT_H

#include <QTextEdit>

class QWidget;
class QKeyEvent;


class TextEdit: public QTextEdit {
    Q_OBJECT

public:
    explicit TextEdit(QWidget* parent = nullptr);

signals:
    void pressedEnter();

protected:
    void keyPressEvent(QKeyEvent* e);
};

#endif // TEXTEDIT_H