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
include(ECG_BASELINE/ecg_baseline.pri)
include(ATRIAL_FIBR/atrial_fibr.pri)
include(ST_INTERVAL/st_interval.pri)

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = EKG
TEMPLATE = app


SOURCES += main.cpp

INCLUDEPATH += Include

QMAKE_CXXFLAGS += -std=c++0x
