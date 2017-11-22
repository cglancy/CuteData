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
    LIBS += -L./debug -lCuteData0
    PRE_TARGETDEPS += ./debug/CuteData0.dll
}
else {
    LIBS += -L./release -lCuteData0
    PRE_TARGETDEPS += ./release/CuteData0.dll
}