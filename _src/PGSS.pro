#---------------------------------------------------
#
#       Propulse Ground Station Software
#
# by Amalie Wee & Ole Bjørn Eithun Pedersen, 2018
#
#---------------------------------------------------

QT       += widgets core gui charts serialport \
            3dcore 3drender 3dinput 3dextras \
            quickwidgets location

TARGET = PGSS
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    realtimechart.cpp \
    serialinterface.cpp \
    xbee.cpp \
    navballwidget.cpp \
    renderableentity.cpp \
    navball.cpp

HEADERS += \
        mainwindow.h \
    realtimechart.h \
    globals.h \
    serialinterface.h \
    xbee.h \
    navballwidget.h \
    renderableentity.h \
    navball.h

RESOURCES += \
    navball.qrc \
    qml.qrc


win32:RC_ICNS = propulse_logo_icon.ico
macx:ICON = propulse_logo.icns
