#-------------------------------------------------
#
# Project created by QtCreator 2015-05-15T23:31:36
#
#-------------------------------------------------

QT       += core gui

QMAKE_CXXFLAGS += -std=gnu++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = fliezzz
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

DISTFILES += \
    docs/tasks.txt
