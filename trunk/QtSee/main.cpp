//
//  main.cpp
//  This file is part of QtSee
//
//  QtSee is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  QtSee is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with QtSee.  If not, see <http://www.gnu.org/licenses/>.
//
//  Written by Alan Mills, Camberley, England
//         and Robin Mills, San Jose, CA, USA.
//  http://clanmills.com
//

#include <QtGui>
#include "mainwindow.h"
#include "platform.h"

int main(int argc, char * argv[])
{
    platformInit(argc,argv);
    QApplication app(argc, argv);
    QUrl url;
    if (argc > 1)
        url = QUrl(argv[1]);
    else
        url = QUrl("http://clanmills.com") ; // "http://www.google.com/ncr");
    MainWindow *browser = new MainWindow(url);
    browser->show();
    return app.exec();
}

// That's all Folks!
////
