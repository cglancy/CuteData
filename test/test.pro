QT       += core sql testlib

TARGET = TestCuteData
CONFIG   += testcase 

TEMPLATE = app

HEADERS +=  comment.h \
	datatypes.h \
	post.h \
	tag.h \
    testcutedata.h \
	user.h \
	userprofile.h
	
SOURCES += comment.cpp \
    main.cpp \
	post.cpp \
	tag.cpp \
    testcutedata.cpp \
	user.cpp \
	userprofile.cpp

INCLUDEPATH += ../src

CONFIG(debug, debug|release) {
    LIBS += -L../src/debug -lCuteData0
    PRE_TARGETDEPS += ../src/debug/CuteData0.dll
}
else {
    LIBS += -L../src/release -lCuteData0
    PRE_TARGETDEPS += ../src/release/CuteData0.dll
}