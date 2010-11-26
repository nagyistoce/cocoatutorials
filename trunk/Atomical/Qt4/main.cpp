//
//  main.cpp
//  This file is part of Atomical
//
//  Atomical is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  Atomical is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with Atomical.  If not, see <http://www.gnu.org/licenses/>.
//
//  Written by Fabio Cavaliere, Genova, Italy
//  Additional Engineering by Robin Mills, San Jose, CA, USA. http://clanmills.com
//

#include <QApplication>
#include <QDesktopWidget>
#include <QtGui/QApplication>
#include <stdio.h>

#include "Qt4.h"
#include "qt4settings.h"
#include "mainwindow.h"
#include "about.h"

Qt4Settings* theSettings;

int main(int argc, char* argv[])
{
    setbuf(stdout,NULL);  // don't buffer stdout so we don't need fflush(stdout) calls!
    platformInit(argc,argv);
    QApplication a(argc, argv);

    theSettings = new Qt4Settings();

    MainWindow mainWindow(MaxNp);
    mainWindow.show();
    int result = a.exec();

    delete theSettings ;
    theSettings = NULL ;

    return result ;
}

// That's all Folks!
////
