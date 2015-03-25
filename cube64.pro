#-------------------------------------------------
#
# Project created by QtCreator 2015-03-16T21:40:48
#
#-------------------------------------------------

QT       += core gui
QT       += serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = cube64
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    cpattern.cpp \
    ccubestate.cpp \
    ccubeview.cpp

HEADERS  += mainwindow.h \
    cpattern.h \
    ccubestate.h \
    ccubeview.h
