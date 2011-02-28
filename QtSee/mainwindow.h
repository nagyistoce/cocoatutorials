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
//         and Robin Mills, San Jose, CA, USA
//  http://clanmills.com
//

#pragma once
#ifndef MainWindow_H_
#define MainWindow_H_

#include <QtGui>
#include <QtGui/QMainWindow>

#ifndef QT_NO_SCRIPTTOOLS
#include <QAction>
#include <QApplication>
#include <QMainWindow>
#include <QPushButton>
#include <QVBoxLayout>
#include <QScriptEngineDebugger>
#endif

#include "ui_mainwindow.h"
#include <vector>

class QWebView;
QT_BEGIN_NAMESPACE
class QLineEdit;
QT_END_NAMESPACE

namespace Ui
{
    class MainWindowClass;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
             MainWindow(const QUrl& url);
    virtual ~MainWindow() { term() ; }
    void     term();

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
    void actionForward();
    void actionBack();
    void actionReload();
    void actionStop();
    void actionFirst();
    void actionLast();
    void actionNext();
    void actionPrev();
    void actionDebug();
    void actionRun();

//  execute commands
    void    command(QString c);
    QString command_help(QString& c);
    QString command_run (QString& c);

//  alert boxes
    void notImplementedYet(const char* s);
    void alert(QString s);

//  execute scripts
    void runScript(const QString& fileName,bool debug);

    //  other methods
    void updatePhotos(QDir& dir,QStringList& photos);
    void showPhoto();

private:
    Ui::MainWindowClass*   ui;
    QScriptEngineDebugger* m_debugger;
    QMainWindow*           m_debugWindow;
    QScriptEngine*         m_engine;

    QString     jQuery;
    QWebView*   view;
    QLineEdit*  locationEdit;
    QAction*    rotateAction;
    int         progress;
    QToolBar*   toolBar;
    bool        bWebView;
    QStringList photos;
    int         next_photo;

    std::vector<QString> tempFiles;
};

#endif

// That's all Folks!
////
