# -------------------------------------------------
# Project created by QtCreator
# -------------------------------------------------
QT          +=  opengl
TARGET       =  Qt4
TEMPLATE     =  app

SOURCES     +=  main.cpp \
                mainwindow.cpp \
                glwidget.cpp \
                about.cpp \
                calcthread.cpp \
                GLThread.cpp \
                platform.cpp \
                preferences.cpp \
                qt4settings.cpp

HEADERS     +=  mainwindow.h \
                glwidget.h \
                about.h \
                calcthread.h \
                GLThread.h \
                platform.h \
                Qt4.h \
                preferences.h \
                qt4settings.h

FORMS       +=  mainwindow.ui \
                about.ui \
                preferences.ui

RESOURCES   +=  res.qrc



win32 {
    DEFINES +=  __WINDOWS__=1
}

unix {
    DEFINES +=  __LINUX__=1
}

OTHER_FILES += \
    ../ReadMe.txt
