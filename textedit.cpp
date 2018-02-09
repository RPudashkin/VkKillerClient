#include <QKeyEvent>
#include "textedit.h"


TextEdit::TextEdit(QWidget* parent): QTextEdit(parent)
{}


void TextEdit::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter)
        emit pressedEnter();
    else QTextEdit::keyPressEvent(event);
}
