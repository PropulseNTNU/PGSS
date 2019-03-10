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
<<<<<<< HEAD
    xbee.cpp
=======
    navballwidget.cpp \
    renderableentity.cpp \
    navball.cpp
>>>>>>> 840a6ed23c66a1a77d4fd42a1b1a1980e3c28bb3

HEADERS += \
        mainwindow.h \
    realtimechart.h \
    globals.h \
    serialinterface.h \
<<<<<<< HEAD
    xbee.h
=======
    navballwidget.h \
    renderableentity.h \
    navball.h

RESOURCES += \
    navball.qrc \
    qml.qrc
>>>>>>> 840a6ed23c66a1a77d4fd42a1b1a1980e3c28bb3

win32:RC_ICNS = propulse_logo_icon.ico
macx:ICON = propulse_logo.icns
