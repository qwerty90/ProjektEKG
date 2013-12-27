TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
SOURCES += \
    main.cpp \
    src/rrintervalmethod.cpp

QMAKE_CXXFLAGS += -std=c++0x

HEADERS += \
    src/rrintervalmethod.h
