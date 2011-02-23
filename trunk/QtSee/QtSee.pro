#
#  QtSee.pro
#  This file is part of QtSee
#
#  QtSee is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  QtSee is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with QtSee.  If not, see <http:#www.gnu.org/licenses/>.
#
#  Written by Alan Mills, Camberley, England
#         and Robin Mills, San Jose, CA, USA
#  http:#clanmills.com
#

TARGET        =  QtSee
TEMPLATE      =  app

QT           +=  webkit  \
                 network \
                 script  \
                 scripttools

HEADERS       =  mainwindow.h \
                 about.h      \
                 platform.h   \
                 QtSee.h \
    File.h \
    Debug.h

SOURCES       =  main.cpp       \
                 mainwindow.cpp \
                 about.cpp      \
                 platform.cpp \
    File.cpp \
    Debug.cpp

RESOURCES     =  jquery.qrc

FORMS         += mainwindow.ui  \
                 preferences.ui \
                 about.ui

OTHER_FILES   += Resources/photo.jpg \
    Resources/photo.jpg \
    Resources/p18.jpg \
    Resources/p17.jpg \
    Resources/p16.jpg \
    Resources/p15.jpg \
    Resources/p14.jpg \
    Resources/p13.jpg \
    Resources/p12.jpg \
    Resources/p11.jpg \
    Resources/p10.jpg \
    Resources/p9.jpg \
    Resources/p8.jpg \
    Resources/p7.jpg \
    Resources/p6.jpg \
    Resources/p5.jpg \
    Resources/p4.jpg \
    Resources/p3.jpg \
    Resources/p2.jpg \
    Resources/p1.jpg \
    docs/RobinsNotes.txt \
    Resources/photo.html

win32 {
    DEFINES      += __WINDOWS__=1
    INCLUDEPATH  += $$quote(C:/MinGW/msys/1.0/local/include)
    LIBS         += $$quote(C:/MinGW/msys/1.0/local/lib/libexiv2.dll.a)
}


unix {
    DEFINES      += __LINUX__=1
    INCLUDEPATH  += /usr/local/include
    LIBS         += -L/usr/local/lib -lexiv2
}

macx {
    ICON          = QtSee.icns
    INCLUDEPATH  += /usr/local/include
    LIBS         += -L/usr/local/lib -lexiv2
}


# install
#target.path   = $$[QT_INSTALL_EXAMPLES]/webkit/QtSee
#sources.files = $$SOURCES $$HEADERS $$RESOURCES *.pro
#sources.path  = $$[QT_INSTALL_EXAMPLES]/webkit/fancybrowser
#INSTALLS     += target sources

# That's all Folks!
##
