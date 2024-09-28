QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++20

INCLUDEPATH+= $$PWD/src

SOURCES += \
    $$PWD/src/appconfig.cpp \
    $$PWD/src/preferencenode.cpp \
    $$PWD/src/preferenceshelper.cpp \
    $$PWD/src/widgetconfig.cpp

HEADERS += \
    $$PWD/src/MyConfig.hpp \
    $$PWD/src/appconfig.h \
    $$PWD/src/preferencenode.h \
    $$PWD/src/preferenceshelper.h \
    $$PWD/src/widgetconfig.h

FORMS +=
