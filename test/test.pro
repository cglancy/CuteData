QT += core sql testlib

TARGET = cgDataTest
CONFIG += testcase 

TEMPLATE = app

HEADERS += datatest.h \
	comment.h \
	datatypes.h \
	post.h \
	tag.h \
	user.h \
	userprofile.h
	
SOURCES += datatest.cpp \
	comment.cpp \
    main.cpp \
	post.cpp \
	tag.cpp \
	user.cpp \
	userprofile.cpp

INCLUDEPATH += ../src

CONFIG(debug, debug|release) {
    LIBS += -L../src/debug -lcgData0
    PRE_TARGETDEPS += ../src/debug/cgData0.dll
}
else {
    LIBS += -L../src/release -lcgData0
    PRE_TARGETDEPS += ../src/release/cgData0.dll
}