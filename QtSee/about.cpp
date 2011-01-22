//
//  about.cpp
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

#include "about.h"
#include "ui_about.h"
#include "platform.h"

about::about(QWidget *parent)
: QDialog(parent)
, m_ui(new Ui::about)
{
    m_ui->setupUi(this);
}

about::~about()
{
    delete m_ui;
}

void about::linkActivated(QString data)
{
    ::LinkActivated(data.toAscii());
}

// That's all Folks!
////

