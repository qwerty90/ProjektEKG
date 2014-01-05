include( $$(QWT_ROOT)/qwtconfig.pri )
include( $$(QWT_ROOT)/qwtfunctions.pri )

SOURCES += \
    Views/airecgmain.cpp \
    Views/about.cpp \
    Views/filebrowser.cpp \
    Views/processing.cpp \
    Views/notready.cpp\
    Views/scrollzoomer.cpp\
    Views/scrollbar.cpp

HEADERS += \
    Views/airecgmain.h \
    Views/about.h \
    Views/filebrowser.h \
    Views/processing.h \
    Views/notready.h \
    Views/scrollzoomer.h\
    Views/scrollbar.h

FORMS    += \
    Views/airecgmain.ui \
    Views/about.ui \
    Views/filebrowser.ui \
    Views/processing.ui \
    Views/notready.ui

RESOURCES += \
    Views/resources.qrc


INCLUDEPATH += $$(QWT_ROOT)/src
DEPENDPATH  += $$(QWT_ROOT)/src

QMAKE_RPATHDIR *= $${QWT_ROOT}/lib

contains(QWT_CONFIG, QwtFramework) {

    LIBS      += -F$$(QWT_ROOT)/lib
}
else {

    LIBS      += -L$$(QWT_ROOT)/lib
}

qwtAddLibrary(qwt)


contains(QWT_CONFIG, QwtSvg) {

    QT += svg
}
else {

    DEFINES += QWT_NO_SVG
}


win32 {
    contains(QWT_CONFIG, QwtDll) {
        DEFINES    += QT_DLL QWT_DLL
    }
}

DEFINES += QWT_DLL
CONFIG += qwt
