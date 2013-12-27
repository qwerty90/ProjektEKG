#-------------------------------------------------
#
# Project created by QtCreator 2013-12-27T16:49:22
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = tst_unitteststest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += tst_unitteststest.cpp \
    ../rrintervalmethod.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

HEADERS += \
    ../src/rrintervalmethod.h
QMAKE_CXXFLAGS += -std=c++0x
