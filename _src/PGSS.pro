#---------------------------------------------------
#
# Project created by Ole Bjørn Eithun Pedersen, 2018
# For Propulse NTNU
#
#---------------------------------------------------

QT       += core gui charts serialport 3dcore 3drender 3dinput 3dextras

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = PGSS
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    realtimechart.cpp \
    serialinterface.cpp \
    xbee.cpp

HEADERS += \
        mainwindow.h \
    realtimechart.h \
    globals.h \
    serialinterface.h \
    xbee.h

win32:RC_ICNS = propulse_logo_icon.ico
macx:ICON = propulse_logo.icns
