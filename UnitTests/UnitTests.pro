QT       += testlib
QT       -= gui

TARGET = tst_RRSanity
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += tst_RRSanity.cpp \
    ../src/RRIntervals.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

HEADERS += \
    ../src/RRIntervals.h

QMAKE_CXXFLAGS += -std=c++0x -Wall -Werror -Wextra
QMAKE_CXXFLAGS += -isystem $$[QT_INSTALL_PREFIX]/include/qt4/QtCore -isystem $$[QT_INSTALL_PREFIX]/include/qt4/QtTest
