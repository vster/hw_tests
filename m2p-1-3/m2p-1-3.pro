TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    m2p-1-3.cpp \
    ../util.cpp

HEADERS += \
    ../util.h

QMAKE_CXXFLAGS += -O0
