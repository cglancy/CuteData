QT       += core sql

TARGET = CuteData
CONFIG += dll
TEMPLATE = lib
VERSION	= 0.1.1

HEADERS += cutedata.h \
	datamanager.h \
    dataobject.h

SOURCES += datamanager.cpp \
    dataobject.cpp

DEFINES += CUTEDATA_EXPORTS