//
//  preferences.cpp
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

#include "preferences.h"
#include "ui_preferences.h"
#include "Qt4.h"

preferences::preferences(QWidget *parent)
: QDialog(parent)
, m_ui(new Ui::preferences)
{
    m_ui->setupUi(this);
}

preferences::~preferences()
{
    delete m_ui;
}

void preferences::linkActivated(QString data)
{
    ::LinkActivated((const char*)data.toAscii());
}

// That's all Folks!
////

