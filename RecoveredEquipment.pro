#-------------------------------------------------
#
# Project created by QtCreator 2016-04-14T21:42:34
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = RecoveredEquipment
TEMPLATE = app


SOURCES += main.cpp\
    CustomControls/porttimetextedit.cpp \
    angle.cpp \
    emg/emg.cpp \
    emg/portchannelattr.cpp \
    mainwindow.cpp \
    IcoHelper/iconhelper.cpp \
    SystemInfo/systeminfo.cpp \
    control.cpp \
    eeg.cpp \
    motorcontrol.cpp \
    setting.cpp \
    qcustomplot.cpp \
    emokit/emokit.cpp \
    emokit/emokit_decode.cpp \
    emokit/qemokit.cpp \
    GPIO/qgpio.cpp \
    QExtSerialPort/posix_qextserialport.cpp \
    QExtSerialPort/qextserialbase.cpp \
    QExtSerialPort/qextserialport.cpp \
    SerialPort/qserialporthelper.cpp \
    FFTW/FFTW.cpp

HEADERS  += mainwindow.h \
    CustomControls/porttimetextedit.h \
    IcoHelper/iconhelper.h \
    SystemInfo/systeminfo.h \
    angle.h \
    control.h \
    eeg.h \
    emg/emg.h \
    emg/portchannelattr.h \
    motorcontrol.h \
    setting.h \
    includes.h \
    qcustomplot.h \
    emokit/emokit.h \
    emokit/emokit_decode.h \
    emokit/qemokit.h \
    GPIO/qgpio.h \
    QExtSerialPort/posix_qextserialport.h \
    QExtSerialPort/qextserialbase.h \
    QExtSerialPort/qextserialport.h \
    SerialPort/qserialporthelper.h \
    FFTW/FFTW.h

FORMS    += mainwindow.ui \
    control.ui \
    eeg.ui \
    emg.ui \
    setting.ui

RESOURCES += \
    res.qrc

LIBS += /usr/lib/libmcrypt.so \
        /usr/lib/aarch64-linux-gnu/libhidapi-hidraw.so \
        /usr/lib/aarch64-linux-gnu/libfftw3.so.3 \
        /usr/local/lib/liblog4cplus.so \
        "/usr/lib/libwiringPi.so"
