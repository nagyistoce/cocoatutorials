//
//  qt4settings.h
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

#pragma once
#ifndef Qt4Settings_H
#define Qt4Settings_H

#include <QApplication>
#include <QSettings>
#include <QVariant>

class Qt4Settings : public QSettings
{
    Q_OBJECT
public:
    explicit Qt4Settings();

public:
    QString    sFullScreenControls ;
    QString    sFullScreenMenubar;
    QString    sNativeDialogs;

    QString    sBackgroundRed;
    QString    sBackgroundGreen;
    QString    sBackgroundBlue;

    QVariant   vTrue;
    QVariant   vFalse;

    QColor     backgroundColor();

signals:

public slots:

};

#endif // Qt4Settings_H

// That's all Folks!
////
