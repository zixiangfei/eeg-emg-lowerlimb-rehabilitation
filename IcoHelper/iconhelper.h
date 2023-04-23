#ifndef ICONHELPER_H
#define ICONHELPER_H

#include <QObject>
#include <QFont>
#include <QFontDatabase>
#include <QMutex>
#include <QLabel>
#include <QPushButton>
#include <QApplication>
#include <QTabWidget>
#include <QGroupBox>
#include <QToolBox>


class IconHelper : public QObject
{
private:
    explicit IconHelper(QObject *parent = 0);
    QFont iconFont;
    static IconHelper* _instance;

public:
    /**
     * @brief Instance
     * @return
     */
    static IconHelper* Instance()
    {
        static QMutex mutex;
        if (!_instance)
        {
            QMutexLocker locker(&mutex);
            if (!_instance)
            {
                _instance = new IconHelper;
            }
        }
        return _instance;
    }

    void SetIcon(QLabel* lab, QChar c, int size = 10);
    void SetIcon(QPushButton* btn, QChar c, int size = 10);
    void SetIcon(QPushButton* btn, QChar c,const QString &str, int size);
    void SetIcon(QTabWidget* btn, int index, QChar c, int size);   
    void SetIcon(QTabWidget* btn, int index, QChar c, const QString &name, int size);
    void SetIcon(QGroupBox* box, QChar c, int size);
    void SetIcon(QGroupBox* box, QChar c, const QString &name, int size);
    void SetIcon(QPushButton* btn, const QString &str, int size);
    void SetIcon(QToolBox* box,int index, QChar c, const QString &name, int size);
};

#endif // ICONHELPER_H
