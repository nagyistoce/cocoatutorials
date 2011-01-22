//
//  mainwindow.h
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
#ifndef MainWindow_H_
#define MainWindow_H_

#include <QtGui>
#include <QtGui/QMainWindow>
#include "ui_mainwindow.h"

class QWebView;
QT_BEGIN_NAMESPACE
class QLineEdit;
QT_END_NAMESPACE

namespace Ui
{
    class MainWindowClass;
}

//! [1]
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(const QUrl& url);

protected slots:

    void adjustLocation();
    void changeLocation();
    void adjustTitle();
    void setProgress(int p);
    void finishLoading(bool);

    void viewSource();
    void slotSourceDownloaded();

    void highlightAllLinks();
    void rotateImages(bool invert);
    void removeGifImages();
    void removeInlineFrames();
    void removeObjectElements();
    void removeEmbeddedElements();

//  menu actions
    void actionOpen();
    void actionSave();
    void actionSaveAs();
    void actionPrint();
    void actionExit();
    void actionCut();
    void actionCopy();
    void actionDelete();
    void actionPaste();
    void actionPreferences();
    void actionFullScreen();
    void actionOnScreenTools();
    void actionAbout();
    void actionHelp();

//  execute a command
    void command(QString c);

//  alert boxes
    void notImplementedYet(const char* s);
    void alert(QString s);

private:
    Ui::MainWindowClass* ui;

    QString     jQuery;
    QWebView*   view;
    QLineEdit*  locationEdit;
    QAction*    rotateAction;
    int         progress;
    QToolBar*   toolBar;
//! [1]
};

#endif

// That's all Folks!
////
