TARGET        =  Qt4
TEMPLATE      =  app

QT           +=  webkit network
HEADERS       =  mainwindow.h

SOURCES       =  main.cpp \
                 mainwindow.cpp

RESOURCES     =  jquery.qrc

FORMS        +=  mainwindow.ui


# install
#target.path   = $$[QT_INSTALL_EXAMPLES]/webkit/QtSee
#sources.files = $$SOURCES $$HEADERS $$RESOURCES *.pro
#sources.path  = $$[QT_INSTALL_EXAMPLES]/webkit/fancybrowser
#INSTALLS     += target sources
