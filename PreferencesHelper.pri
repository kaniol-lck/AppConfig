QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++20

INCLUDEPATH+= $$PWD/src

SOURCES += \
    $$PWD/src/appconfig.cpp \
    $$PWD/src/widgetconfig.cpp

HEADERS += \
    $$PWD/src/MyConfig.hpp \
    $$PWD/src/appconfig.h \
    $$PWD/src/widgetconfig.h

FORMS +=
