TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    p2m-1-3.cpp \
    ../util.cpp

HEADERS += \
    ../util.h

QMAKE_CXXFLAGS += -O0
