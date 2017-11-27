QT       += core sql

TARGET = cgData
CONFIG += dll
TEMPLATE = lib
VERSION	= 0.1.1

HEADERS += cgdata.h \
	datamanager.h \
    dataobject.h

SOURCES += datamanager.cpp \
    dataobject.cpp

DEFINES += CGDATA_EXPORTS