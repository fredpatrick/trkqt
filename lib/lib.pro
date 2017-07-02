#-------------------------------------------------
#
# Project created by QtCreator 2016-12-08T14:53:46
#
#-------------------------------------------------

QT       += core gui widgets network

TARGET = trkqt
TEMPLATE = lib

DEFINES += LIBTRKQT_LIBRARY

INCLUDEPATH += ../include
INCLUDEPATH += ../../trku/include

LIBS += -L/Users/fredpatrick/wrk/trku/lib -ltrku0.0
OBJECTS_DIR = objects
MOC_DIR     = mocs

SOURCES += ../cpp/gllayoutshader.cpp\
           ../cpp/glaxesshader.cpp\
           ../cpp/glbedlineshader.cpp\
           ../cpp/gllayoutwidget.cpp\
           ../cpp/modelcoordinates.cpp\
           ../cpp/ranges.cpp\
           ../cpp/tslider.cpp\
           ../cpp/layoutgeometry.cpp\
           ../cpp/layoutstate.cpp\
           ../cpp/zonegeometry.cpp\
           ../cpp/sectiongeometry.cpp\
           ../cpp/shellgeometry.cpp\
           ../cpp/bedlinegeometry.cpp\
           ../cpp/socketclient.cpp\
           ../cpp/switchgeometry.cpp\
           ../cpp/switch.cpp\
           ../cpp/switcher.cpp\
           ../cpp/switches.cpp\
           ../cpp/path.cpp\
           ../cpp/paths.cpp\
           ../cpp/pathnode.cpp\
           ../cpp/paintlist.cpp

HEADERS += ../include/libtrkqt_global.h\
           ../include/gllayoutshader.h\
           ../include/glaxesshader.h\
           ../include/glbedlineshader.h\
           ../include/gllayoutwidget.h\
           ../include/modelcoordinates.h\
           ../include/ranges.h\
           ../include/tslider.h\
           ../include/layoutgeometry.h\
           ../include/layoutstate.h\
           ../include/zonegeometry.h\
           ../include/sectiongeometry.h\
           ../include/shellgeometry.h\
           ../include/bedlinegeometry.h\
           ../include/socketclient.h\
           ../include/switchgeometry.h\
           ../include/switch.h\
           ../include/switcher.h\
           ../include/switches.h\
           ../include/path.h\
           ../include/paths.h\
           ../include/pathnode.h\
           ../include/paintlist.h


unix {
    target.path = /usr/lib
    INSTALLS += target
}
