TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    ../util.cpp \
    m2m-1-3.cpp

HEADERS += \
    ../util.h

QMAKE_CXXFLAGS += -O0
