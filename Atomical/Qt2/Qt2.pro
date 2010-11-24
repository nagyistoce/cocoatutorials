#-------------------------------------------------
#
# Project created by QtCreator 2010-10-20T19:35:50
#
#-------------------------------------------------

QT            += core gui opengl

TARGET        = Qt2
TEMPLATE      = app

HEADERS       = glwidget.h \
                window.h \
                Qt2.h \
                calcthread.h \
                GLThread.h \
                platform.h

SOURCES       = glwidget.cpp \
                main.cpp \
                window.cpp \
                calcthread.cpp \
                GLThread.cpp \
                platform.cpp

OTHER_FILES  += \
                ../ReadMe.txt

win32 {
    DEFINES  += __WINDOWS__=1
}

unix {
    DEFINES  += __LINUX__=1
}