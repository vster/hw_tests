TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    m2p-test-1-0.cpp \
    util.cpp

HEADERS += \
    util.h

QMAKE_CXXFLAGS += -O0
