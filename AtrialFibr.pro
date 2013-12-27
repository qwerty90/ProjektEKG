TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
SOURCES += \
    main.cpp \
    rrintervalmethod.cpp

QMAKE_CXXFLAGS += -std=c++0x

HEADERS += \
    rrintervalmethod.h
