#-------------------------------------------------
#
# Project created by QtCreator 2010-10-20T19:35:50
#
#-------------------------------------------------

QT       += core gui opengl

TARGET = Qt2
TEMPLATE = app


VPATH += ../shared
INCLUDEPATH += ../shared

HEADERS       = glwidget.h \
                window.h \
    ../Fabio/ranmar.h \
    ../Fabio/nrutil.h \
    ../Fabio/jacobi.h \
    ../Fabio/globals.h \
    ../Fabio/OGLView.h \
    Qt2.h

SOURCES       = glwidget.cpp \
                main.cpp \
                window.cpp \
    ../Fabio/ranmar.c \
    ../Fabio/nrutil.c \
    ../Fabio/jacobi.c \
    ../Fabio/OGLView.cpp

OTHER_FILES += \
    ../ReadMe.txt
