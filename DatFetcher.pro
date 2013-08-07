#-------------------------------------------------
#
# Project created by QtCreator 2013-08-06T10:16:09
#
#-------------------------------------------------

QT       += core gui sql network

QTPLUGIN += qsqlite

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = DatFetcher
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui
