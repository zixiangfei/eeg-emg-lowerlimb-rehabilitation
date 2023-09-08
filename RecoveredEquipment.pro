#-------------------------------------------------
#
# Project created by QtCreator 2016-04-14T21:42:34
#
#-------------------------------------------------

QT       += core gui
QT       += serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = RecoveredEquipment
TEMPLATE = app


SOURCES += main.cpp\
    CustomControls/porttimetextedit.cpp \
    PythonGIL/pythreadstatelock.cpp \
    angle.cpp \
    emg/emg.cpp \
    emg/emgserialport.cpp \
    emg/portchannelattr.cpp \
    emgdataget.cpp \
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
    PythonGIL/pythreadstatelock.h \
    SystemInfo/systeminfo.h \
    angle.h \
    control.h \
    eeg.h \
    emg/emg.h \
    emg/emgserialport.h \
    emg/portchannelattr.h \
    emgdataget.h \
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

INCLUDEPATH += /usr/include/python3.9

LIBS += /usr/lib/libmcrypt.so \
        /usr/lib/aarch64-linux-gnu/libhidapi-hidraw.so \
        /usr/lib/aarch64-linux-gnu/libfftw3.so.3 \
        /usr/local/lib/liblog4cplus.so \
        "/usr/lib/libwiringPi.so" \
        /usr/lib/aarch64-linux-gnu/libpython3.9.so

DISTFILES += \
    PythonCode/establishConnect.py \
    PythonCode/extract_feature.py \
    PythonCode/neuracle_lib/__init__.py \
    PythonCode/neuracle_lib/__pycache__/__init__.cpython-37.pyc \
    PythonCode/neuracle_lib/__pycache__/__init__.cpython-38.pyc \
    PythonCode/neuracle_lib/__pycache__/__init__.cpython-39.pyc \
    PythonCode/neuracle_lib/__pycache__/dataServer.cpython-37.pyc \
    PythonCode/neuracle_lib/__pycache__/dataServer.cpython-38.pyc \
    PythonCode/neuracle_lib/__pycache__/dataServer.cpython-39.pyc \
    PythonCode/neuracle_lib/__pycache__/readbdfdata.cpython-37.pyc \
    PythonCode/neuracle_lib/__pycache__/triggerBox.cpython-37.pyc \
    PythonCode/neuracle_lib/__pycache__/triggerBox.cpython-39.pyc \
    PythonCode/neuracle_lib/dataServer.py \
    PythonCode/neuracle_lib/readbdfdata.py \
    PythonCode/neuracle_lib/triggerBox.py \
    PythonCode/online_fetch_data.py \
    PythonCode/svm_model.pkl
