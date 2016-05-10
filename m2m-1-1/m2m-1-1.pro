TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    m2m-1-1.cpp \
    util.cpp

HEADERS += \
    util.h

QMAKE_CXXFLAGS += -O0
