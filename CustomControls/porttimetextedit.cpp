#include "porttimetextedit.h"

portTimeTextEdit::portTimeTextEdit(QWidget *parent) : QTextEdit(parent) {

}

portTimeTextEdit::~portTimeTextEdit() {

}

void portTimeTextEdit::focusInEvent(QFocusEvent *e) {
    if(e) {
        return;
    }
}

void portTimeTextEdit::focusOutEvent(QFocusEvent *e) {
    if(e) {
        return;
    }
}
