#-------------------------------------------------
#
# Project created by QtCreator 2014-04-15T09:40:35
#
#-------------------------------------------------



CONFIG +=c++11
QT = core
QT +=widgets

greaterThan(QT_MAJOR_VERSION, 4) {
    QT       += serialport
} else {
    include($$QTSERIALPORT_PROJECT_ROOT/src/serialport/qt4support/serialport.prf)
}

ICON = iconflash.icns

TARGET = "Airboxlab flashing tool"
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    dfuthread.cpp \
    portthread.cpp

HEADERS  += mainwindow.h \
    dfuthread.h \
    portthread.h

FORMS    += mainwindow.ui

OTHER_FILES +=

RESOURCES +=


APP_FIRMWARE_FILES.files = Firmware
APP_FIRMWARE.path = Contents/MacOS

QMAKE_BUNDLE_DATA += APP_FIRMWARE_FILES



