#ifndef PORTTIMETEXTEDIT_H
#define PORTTIMETEXTEDIT_H

#include <QWidget>
#include <QTextEdit>

class portTimeTextEdit : public QTextEdit
{
    Q_OBJECT
public:
    explicit portTimeTextEdit(QWidget *parent = nullptr);
    ~portTimeTextEdit();

protected:
    virtual void focusInEvent(QFocusEvent *e);
    virtual void focusOutEvent(QFocusEvent *e);

signals:

};

#endif // PORTTIMETEXTEDIT_H
