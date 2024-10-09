QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++20

INCLUDEPATH+= $$PWD/src

SOURCES += \
    $$PWD/src/appconfig.cpp

HEADERS += \
    $$PWD/src/appconfig.h \
    $$PWD/src/configwrapperwidget.h

FORMS +=
