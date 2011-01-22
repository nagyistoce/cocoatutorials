//
//  preferences.h
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
//  Additional Engineering by Robin Mills, San Jose, CA, USA.
//  http://clanmills.com
//

#pragma once
#ifndef PREFERENCES_H
#define PREFERENCES_H

#include <QtGui>
#include <QDialog>

class MainWindow;// forward

namespace Ui
{
    class Preferences;
}

class Preferences : public QDialog
{
    Q_OBJECT
    Q_DISABLE_COPY(Preferences)
public:
    explicit Preferences(QWidget* parent = 0);
    virtual ~Preferences();

public slots:
    void linkActivated(QString data);
    void showColor();
    void fullScreenControls();
    void fullScreenMenubar();
    void nativeDialogs();
    void syncBackgroundColor();

private:
    Ui::Preferences* ui;
    MainWindow* mainWindow;
    friend class MainWindow;

};

#endif // PREFERENCES_H

// That's all Folks!
////
