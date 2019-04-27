TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        main.cpp \
        md5.cpp

HEADERS += \
    hashtable.h \
    huff.h \
    lzw.h \
    md5.h

DISTFILES += \
    commands.sh
