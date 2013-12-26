#-------------------------------------------------
#
# Project created by QtCreator 2013-12-03T20:36:27
#
#-------------------------------------------------

QT       += core gui


include(Common/common.pri)
include(Controllers/controllers.pri)
include(Views/views.pri)
include(QsLog/QsLog.pri)
include(ECG_Baseline/ecg_baseline.pri)

#include(ECG_BASELINE/ecg_baseline.pri)

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = EKG
TEMPLATE = app


SOURCES += main.cpp\
    data_input.cpp

#LIBS += -L$$PWD/Libraries/ -lECG_BASELINE

#dlls.path = $$OUT_PWD/$$DESTDIR
#dlls.files += $$files($$PWD/Libraries/*.dll)
INSTALLS += dlls

HEADERS  += data_input.h

INCLUDEPATH += Include

