#-------------------------------------------------
#
# Project created by QtCreator 2014-04-06T00:24:08
#
#-------------------------------------------------

QT       += core gui

TARGET = sources
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    recognition.cpp \
    processing_image.cpp \
    mainwindow_slots.cpp \
    paintarea.cpp \
    genetic_teacher.cpp \
    genetic.cpp

HEADERS  += mainwindow.h \
    recognition.h \
    processing_image.h \
    paintarea.h \
    genetic.h

FORMS    += mainwindow.ui

PKGCONFIG += opencv
CONFIG += link_pkgconfig
