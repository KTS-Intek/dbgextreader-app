#-------------------------------------------------
#
# Project created by QtCreator 2017-04-05T17:47:43
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = dbgextreader
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# Default rules for deployment.
#qnx: target.path = /tmp/$${TARGET}/bin
#else: unix:!android:{
# target.path = /home/sonya/qtapps/$${TARGET}

#}
#!isEmpty(target.path): INSTALLS += target

linux-g++{
target.path = /home/sonya/qtapps/$${TARGET}
INSTALLS += target
}

SOURCES += main.cpp\
        mainwindow.cpp \
    dbgextsocket.cpp

HEADERS  += mainwindow.h \
    dbgextsocket.h

FORMS    += mainwindow.ui

RESOURCES += \
    font.qrc
