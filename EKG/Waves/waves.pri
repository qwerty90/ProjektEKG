INCLUDES += \
    Waves \

HEADERS += \
    Waves/src/waves.h \
    Waves/3rdparty/kiss_fft130/kiss_fft.h \
    Waves/3rdparty/kiss_fft130/_kiss_fft_guts.h \
    Waves/3rdparty/kiss_fft130/tools/kiss_fftr.h

SOURCES += \
    Waves/src/waves.cpp \
    Waves/src/ht.cpp \
    Waves/src/fft_filter.cpp \
    Waves/src/envelope.cpp \
    Waves/3rdparty/kiss_fft130/kiss_fft.c \
    Waves/3rdparty/kiss_fft130/tools/kiss_fftr.c
