TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    m2m-1-2.cpp \
    ../util.cpp

QMAKE_CXXFLAGS += -O0

INCLUDEPATH += \
    ..
