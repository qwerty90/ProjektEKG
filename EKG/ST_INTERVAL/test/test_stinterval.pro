#-------------------------------------------------
#
# Project created by QtCreator 2014-01-07T19:11:06
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = tst_testecgstinterval
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

SOURCES += tst_testecgstinterval.cpp \
    ../ecgstanalyzer.cpp \
    ../ecgutils.cpp

DEFINES += SRCDIR=\\\"$$PWD/\\\"

HEADERS += \
    ../ecgstdata.h \
    ../ecgstanalyzer.h \
    ../ecgutils.h
