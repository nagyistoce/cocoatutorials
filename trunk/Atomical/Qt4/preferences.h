//
//  preferences.h
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
#ifndef PREFERENCES_H
#define PREFERENCES_H

#include <QtGui>
#include <QDialog>

namespace Ui {
    class preferences;
}

class preferences : public QDialog {
    Q_OBJECT
    Q_DISABLE_COPY(preferences)
public:
    explicit preferences(QWidget* parent = 0);
    virtual ~preferences();

public slots:
    void linkActivated(QString data);
    void showColor();


private:
    Ui::preferences* m_ui;
    bool bNative;
};

#endif // PREFERENCES_H

// That's all Folks!
////
