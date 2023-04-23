/*************************************************************************************************
Copyright (C),  2015,  HangZhou DianZi University.
File name:  iconhelper.cpp  iconhelper.h
Author:  zlj 	  Version: V1.0    Date: 2015-11-10
Description: iconhelper


Others:

History:
      Date:        Author:       Version：         Modification:
1.  2015-11-10      zlj			   V1.0        		  建库
*************************************************************************************************/
#include "iconhelper.h"

IconHelper* IconHelper::_instance = 0;

IconHelper::IconHelper(QObject*):
    QObject(qApp)
{
    int fontId = QFontDatabase::addApplicationFont("://FontAwesome/fontawesome-webfont.ttf");
    QString fontName = QFontDatabase::applicationFontFamilies(fontId).at(0);
    iconFont = QFont(fontName);
}

/**
 * @brief IconHelper::SetIcon
 * @param lab
 * @param c
 * @param size
 */
void IconHelper::SetIcon(QLabel* lab, QChar c, int size)
{
    iconFont.setPointSize(size);
    lab->setFont(iconFont);
    lab->setText(c);
}

/**
 * @brief IconHelper::SetIcon
 * @param btn
 * @param c
 * @param size
 */
void IconHelper::SetIcon(QPushButton* btn, QChar c, int size)
{
    iconFont.setPointSize(size);
    btn->setFont(iconFont);
    btn->setText(c);
}

/**
 * @brief IconHelper::SetIcon
 * @param btn
 * @param c
 * @param str
 * @param size
 */
void IconHelper::SetIcon(QPushButton* btn, QChar c, const QString &str, int size)
{
    QString buf = "   ";
    buf.append(str);

    iconFont.setPointSize(size);
    btn->setFont(iconFont);
    btn->setText(c+buf);
}

void IconHelper::SetIcon(QPushButton* btn, const QString &str, int size)
{
    QString buf = "";
    buf.append(str);

    iconFont.setPointSize(size);
    btn->setFont(iconFont);
    btn->setText(buf);
}
/**
 * @brief IconHelper::SetIcon
 * @param btn
 * @param index
 * @param c
 * @param size
 */
void IconHelper::SetIcon(QTabWidget* btn, int index, QChar c, int size)
{
    iconFont.setPointSize(size);
    btn->setFont(iconFont);
    btn->setTabText(index,c);
}

/**
 * @brief IconHelper::SetIcon
 * @param btn
 * @param index
 * @param c
 * @param name
 * @param size
 */
void IconHelper::SetIcon(QTabWidget* btn, int index, QChar c, const QString &name, int size)
{
    QString str = "\n";
    str.append(name);

    iconFont.setPointSize(size);
    btn->setFont(iconFont);
    btn->setTabText(index,c+str);
}

void IconHelper::SetIcon(QGroupBox* box, QChar c, int size)
{
    iconFont.setPointSize(size);
    box->setFont(iconFont);
    box->setTitle(c);
}

void IconHelper::SetIcon(QGroupBox* box, QChar c, const QString &name, int size)
{
    QString str = "  ";
    str.append(name);

    iconFont.setPointSize(size);
    box->setFont(iconFont);
    box->setTitle(c+str);
}

void IconHelper::SetIcon(QToolBox* box,int index, QChar c, const QString &name, int size)
{
    QString str = "  ";
    str.append(name);

    iconFont.setPointSize(size);
    box->setFont(iconFont);
    box->setItemText(index,c+str);
}
