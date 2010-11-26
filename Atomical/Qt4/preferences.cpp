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
#include "mainwindow.h"
#include "Qt4.h"
#include "qt4settings.h"

#define B(b) ((b) ? Qt::Checked:Qt::Unchecked)

Preferences::Preferences(QWidget* parent)
: QDialog(parent)
, ui(new Ui::Preferences)
{
    ui->setupUi(this);
    mainWindow = (MainWindow*) parent;
    ui->fullScreenControls->setCheckState(B(mainWindow->bFullScreenControls ));
    ui->fullScreenMenubar ->setCheckState(B(mainWindow->bFullScreenMenubar  ));
    ui->nativeDialogs     ->setCheckState(B(mainWindow->bNativeDialogs      ));
    syncBackgroundColor();
}

Preferences::~Preferences()
{
    delete ui;
}

void Preferences::syncBackgroundColor()
{
    QColor bg = mainWindow->getBackgroundColor();
    char style[200];
    sprintf(style,"background:#%02x%02x%02x;border:2px solid black;",bg.red(),bg.green(),bg.blue());
    Printf("style = %s\n",style);
    ui->backgroundColorGraphics->setStyleSheet(QString(style));
}

void Preferences::linkActivated(QString data)
{
    ::LinkActivated((const char*)data.toAscii());
}

void Preferences::showColor()
{
    QColor color;
    mainWindow->getBackground(color);
    Printf("mainWindow->bNativeDialogs = %d\n",mainWindow->bNativeDialogs);
    if ( mainWindow->bNativeDialogs )
        color = QColorDialog::getColor(color, this);
    else
        color = QColorDialog::getColor(color, this, "Select Color", QColorDialog::DontUseNativeDialog);

    mainWindow->setBackground(color);
    syncBackgroundColor();


    theSettings->setValue(theSettings->sBackgroundRed  ,QVariant(color.red()  ) );
    theSettings->setValue(theSettings->sBackgroundGreen,QVariant(color.green()) );
    theSettings->setValue(theSettings->sBackgroundBlue ,QVariant(color.blue() ) );
}

void Preferences::fullScreenControls()
{
    mainWindow->fullScreenControls(ui->fullScreenControls->checkState()!=Qt::Unchecked);
}

void Preferences::fullScreenMenubar()
{
    mainWindow->fullScreenMenubar(ui->fullScreenMenubar->checkState()!=Qt::Unchecked);
}

void Preferences::nativeDialogs()
{
    mainWindow->nativeDialogs(ui->nativeDialogs->checkState()!=Qt::Unchecked);
}


// That's all Folks!
////

