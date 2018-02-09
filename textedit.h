#ifndef TEXTEDIT_H
#define TEXTEDIT_H

#include <QTextEdit>


/*
 * TextEdit with support sending a message
 * after key enter pressed
*/
class TextEdit: public QTextEdit {
    Q_OBJECT

public:
    explicit TextEdit(QWidget* parent = nullptr);

signals:
    void pressedEnter();

private:
    void keyPressEvent(QKeyEvent* event);
};

#endif // TEXTEDIT_H