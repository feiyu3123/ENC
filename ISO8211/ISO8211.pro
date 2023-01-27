#-------------------------------------------------
#
# Project created by QtCreator 2023-01-05T20:30:53
#
#-------------------------------------------------

QT       -= gui

TARGET = ISO8211
TEMPLATE = lib

DEFINES += ISO8211_LIBRARY

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
        iso8211.cpp

HEADERS += \
        iso8211.h \
        iso8211_global.h 



unix {
    target.path = /usr/lib
    INSTALLS += target
}



win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../bin/release/ -lUtils
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../bin/debug/ -lUtils
else:unix: LIBS += -L$$PWD/../bin/ -lUtils




