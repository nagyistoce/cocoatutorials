TARGET        =  QtSee
TEMPLATE      =  app

QT           +=  webkit network
HEADERS       =  mainwindow.h \
                 about.h \
                 platform.h \
                 QtSee.h

SOURCES       =  main.cpp \
                 mainwindow.cpp \
                 about.cpp \
                 platform.cpp

RESOURCES     =  jquery.qrc

FORMS         += mainwindow.ui \
                 preferences.ui \
                 about.ui

OTHER_FILES   += Resources/photo.jpg \
    Resources/photo.jpg

win32 {
    DEFINES +=  __WINDOWS__=1
}

unix {
    DEFINES +=  __LINUX__=1
}


# install
#target.path   = $$[QT_INSTALL_EXAMPLES]/webkit/QtSee
#sources.files = $$SOURCES $$HEADERS $$RESOURCES *.pro
#sources.path  = $$[QT_INSTALL_EXAMPLES]/webkit/fancybrowser
#INSTALLS     += target sources
