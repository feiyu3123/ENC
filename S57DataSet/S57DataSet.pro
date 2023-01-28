#-------------------------------------------------
#
# Project created by QtCreator 2023-01-06T19:02:39
#
#-------------------------------------------------

QT       -= gui

TARGET = S57DataSet
TEMPLATE = lib

DEFINES += S57DATASET_LIBRARY \
    QT_NO_DEBUG_OUTPUT

DEFINES -= UNICODE
DEFINES += UMBCS
QMAKE_CXXFLAGS -= -Zc:strictStrings

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    s57geometry.cpp \
    s57meta.cpp \
    s57dataset.cpp \
    s57feature.cpp \
    s57spatial.cpp \
    s57type.cpp

HEADERS += \
    s57geometry.h \
    s57meta.h \
    s57type.h \
    s57dataset.h \
    s57dataset_global.h \
    s57feature.h \
    s57spatial.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../bin/release/
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../bin/debug/
else:unix: LIBS += -L$$PWD/../bin/
LIBS+= -lISO8211 \
 -lUtils


