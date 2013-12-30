QT       += testlib
QT       -= gui

TARGET = AfTests
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += AfTests.cpp \
    ../src/RRIntervals.cpp \
    ../src/PWave.cpp \
    ../src/RRIntervalsApi.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

HEADERS += \
    ../src/RRIntervals.h \
    ../src/PWave.h \
    ../src/RRIntervalsApi.h

QMAKE_CXXFLAGS += -std=c++0x -Wall -Werror -Wextra
QMAKE_CXXFLAGS += -isystem $$[QT_INSTALL_PREFIX]/include/qt4/QtCore -isystem $$[QT_INSTALL_PREFIX]/include/qt4/QtTest
