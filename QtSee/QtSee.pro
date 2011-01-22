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
#  Additional Engineering by Robin Mills, San Jose, CA, USA.
#  http:#clanmills.com
#

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

# That's all Folks!
##