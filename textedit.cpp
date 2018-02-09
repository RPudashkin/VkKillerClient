#include <QKeyEvent>
#include <QWidget>
#include "textedit.h"


TextEdit::TextEdit(QWidget* parent): QTextEdit(parent)
{}


void TextEdit::keyPressEvent(QKeyEvent* e) {
    if (e->key() == Qt::Key_Return || e->key() == Qt::Key_Enter) {
        emit pressedEnter();
    }
}
