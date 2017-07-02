#-------------------------------------------------
#
# Project created by QtCreator 2016-12-09T16:05:10
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

INCLUDEPATH = "/Users/fredpatrick/wrk/trku/include"
INCLUDEPATH += "/Users/fredpatrick/wrk/trkqt/include"
INCLUDEPATH += "/Users/fredpatrick/wrk/trkqt/trkcontroler/include"

LIBS += -L/Users/fredpatrick/wrk/trku/lib -ltrku0.0\
        -L/Users/fredpatrick/wrk/trkqt/lib -ltrkqt

TARGET = trkcontroler
TEMPLATE = app

OBJECTS_DIR = objects
MOC_DIR     = mocs

SOURCES += cpp/main.cpp\
          cpp/trkcontroler.cpp\
          cpp/switchclient.cpp\
          cpp/switchbox.cpp\
          cpp/blockclient.cpp\
          cpp/blockbox.cpp\
          cpp/eventlog.cpp\
          cpp/trkviewer.cpp

HEADERS  += include/trkcontroler.h\
            include/switchclient.h\
            include/switchbox.h\
            include/blockclient.h\
            include/blockbox.h\
            include/eventlog.h\
            include/trkviewer.h
