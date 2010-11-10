# -------------------------------------------------
# Project created by QtCreator 2009-03-19T00:46:57
# -------------------------------------------------
QT 		+= opengl
TARGET 	 = Qt4
TEMPLATE = app

SOURCES +=  main.cpp \
            mainwindow.cpp \
            glwidget.cpp \
            about.cpp \
            calcthread.cpp \
            rendthread.cpp \
            GLThread.cpp \
            platform.cpp

HEADERS +=  mainwindow.h \
            glwidget.h \
            about.h \
            calcthread.h \
            rendthread.h \
            GLThread.h \
            platform.h

FORMS +=    mainwindow.ui \
            about.ui

RESOURCES += res.qrc



win32 {
    DEFINES  += __WINDOWS__=1
}

unix {
    DEFINES  += __LINUX__=1
}
