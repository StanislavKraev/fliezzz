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
    engine/creature.cpp \
    engine/creatureai.cpp \
    engine/field.cpp \
    engine/gamemanager.cpp \
    proto/protomedia.cpp \
    ui/mainwindow.cpp \
    proto/iprotonode.cpp \
    engine/fly.cpp \
    engine/flyai.cpp \
    engine/igamedataprovider.cpp \
    proto/customexceptions.cpp

HEADERS  += mainwindow.h \
    engine/creature.h \
    engine/creatureai.h \
    engine/field.h \
    engine/gamemanager.h \
    proto/protomedia.h \
    ui/mainwindow.h \
    proto/iprotonode.h \
    proto/commandtype.h \
    proto/iprotomedia.h \
    proto/commanddata.h \
    engine/fly.h \
    engine/flyai.h \
    engine/igamedataprovider.h \
    engine/movedirection.h \
    engine/gamestatus.h \
    proto/customexceptions.h

FORMS    += ui/mainwindow.ui

DISTFILES += \
    docs/tasks.txt
