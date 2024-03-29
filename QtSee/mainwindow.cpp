//
//  mainwindow.cpp
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

#include <QtGui>
#include <QtWebKit>
#include <QMessageBox>
#include <QTemporaryFile>

#include "QtSee.h"
#include "mainwindow.h"
#include "about.h"
#include "preferences.h"
#include "ui_mainwindow.h"

#include "File.h"
#include "Debug.h"
#include <exiv2/exiv2.hpp>

#include <iostream>
#include <iomanip>
#include <cassert>

MainWindow::MainWindow(const QUrl& url)
: ui(new Ui::MainWindowClass)
, m_debugger(NULL)
, m_debugWindow(NULL)
, m_engine(NULL)
, bWebView(true)
, next_photo(0)
{
    progress = 0;
    ui->setupUi(this);

#if __APPLE__
    QIcon icon(":/Contents/Resources/QtSee.icns");
    this->setWindowIcon(icon);
#endif

    QFile file;
    file.setFileName(":/jquery.min.js");
    file.open(QIODevice::ReadOnly);
    jQuery = file.readAll();
    file.close();

    QNetworkProxyFactory::setUseSystemConfiguration(true);

    view = ui->webView ;
    view->load(url);
    connect(view, SIGNAL(loadFinished(bool)), SLOT(adjustLocation()));
    connect(view, SIGNAL(titleChanged(QString)), SLOT(adjustTitle()));
    connect(view, SIGNAL(loadProgress(int)), SLOT(setProgress(int)));
    connect(view, SIGNAL(loadFinished(bool)), SLOT(finishLoading(bool)));

    locationEdit = new QLineEdit(this);
    locationEdit->setSizePolicy(QSizePolicy::Expanding, locationEdit->sizePolicy().verticalPolicy());
    connect(locationEdit, SIGNAL(returnPressed()), SLOT(changeLocation()));

    toolBar = addToolBar(tr("Navigation"));
    //toolBar->addAction(icon,"back",self,actionBack);

    toolBar->addAction(view->pageAction(QWebPage::Back));
    toolBar->addAction(view->pageAction(QWebPage::Forward));
    toolBar->addAction(view->pageAction(QWebPage::Reload));
    toolBar->addAction(view->pageAction(QWebPage::Stop));

    //toolBar->addAction(actionBack);
    //toolBar->addAction(actionForward);
    //toolBar->addAction(actionReload);
    //toolBar->addAction(actionStop);

    toolBar->addWidget(locationEdit);

    QMenu *viewMenu = ui->menuView ;
    QAction* viewSourceAction = new QAction("Page Source", this);
    connect(viewSourceAction, SIGNAL(triggered()), SLOT(viewSource()));
    viewMenu->addAction(viewSourceAction);

    QMenu *effectMenu = ui->menuEffects;
    effectMenu->addAction("Highlight all links", this, SLOT(highlightAllLinks()));

    rotateAction = new QAction(this);
    rotateAction->setIcon(style()->standardIcon(QStyle::SP_FileDialogDetailedView));
    rotateAction->setCheckable(true);
    rotateAction->setText(tr("Turn images upside down"));
    connect(rotateAction, SIGNAL(toggled(bool)), this, SLOT(rotateImages(bool)));
    effectMenu->addAction(rotateAction);

    QMenu *toolsMenu = ui->menuTools ;
    toolsMenu->addAction(tr("Remove GIF images"), this, SLOT(removeGifImages()));
    toolsMenu->addAction(tr("Remove all inline frames"), this, SLOT(removeInlineFrames()));
    toolsMenu->addAction(tr("Remove all object elements"), this, SLOT(removeObjectElements()));
    toolsMenu->addAction(tr("Remove all embedded elements"), this, SLOT(removeEmbeddedElements()));
    setCentralWidget(view) ;
    setUnifiedTitleAndToolBarOnMac(true);
}

void MainWindow::term()
{
    ::Printf("good night!");
    if ( tempFiles.size() ) {
        for ( unsigned p = 0 ; p < tempFiles.size() ; p ++) {
            const char* filename = SS(tempFiles[p]);
            ::remove(filename);
            // Printf("deleting temporary file '%s'\n",filename);
        }
        tempFiles.clear();
    }
}

void MainWindow::alert(QString s)
{
    QMessageBox box(this);
    box.setWindowTitle("QtSee Alert");
    box.setText(s);
    box.exec();
}

void MainWindow::notImplementedYet(const char* s)
{
    QString result;
    QTextStream(&result) << "<center><p>Command not implement yet!&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<br>"
                         << "<b>" << QString(s) << "</b></center>";
    alert(result);
}

void MainWindow::updatePhotos(QDir& dir,QStringList& photos)
{
    // sort options
    QDir::Filters   filter = QDir::NoDotAndDotDot | QDir::AllEntries | QDir::QDir::AllDirs ;
    QDir::SortFlags sort   = QDir::Time|QDir::Reversed ;
    QStringList     name   ;
                    name   << "*.jpg" << "*.gif" << "*.tiff" << "*.bmp";

    // read the directory
    dir.setSorting(sort);
    dir.setFilter (filter);
    dir.setNameFilters(name);

    QFileInfoList files  = dir.entryInfoList() ;

    // copy the file paths to the photos list
    for ( int p = 0 ; p < files.size() ; p++ ) {
        QFileInfo file = files[p];
        QString   path = file.absoluteFilePath();
        if ( file.isDir() ) {
            QDir d(path);
            updatePhotos(d,photos);
        } else {
            photos << path;
            Printf("adding %s\n",SS(path));
        }
    }

}

void MainWindow::actionOpen()
{
    // get the user to select a directory
    // I've tried a lot of other stuff here
    // I really wanted to be able to select one or more files/directories
    // however, I haven't found a way to do this
    // I've left the "dead" code below for another day
    QString dirpath = QFileDialog::getExistingDirectory
                    ( this
                    , tr("Open Directory")
                    , QDir::homePath()
                    , QFileDialog::ShowDirsOnly
                    );
    if ( dirpath.length() ) {
        QDir    dir(dirpath);
        updatePhotos(dir,photos);
        showPhoto();
    }

#if 0
    photos.clear();
    next_photo=0 ;
    QStringList files = QFileDialog::getOpenFileNames
                    ( this
                    , tr("Select photos or directories")
                    , QDir::homePath()
                    );
    for ( int f = 0 ; f < files.length() ; f++ ) {
        QString file = files[f] ;
        if ( QFileInfo(file).isDir() ) {
            QDir dir(file);
            updatePhotos(dir,photos);
        } else {
            photos << file ;
        }
    }
    showPhoto();
#endif

#if 0
    QFileDialog dialog;
    dialog.setFileMode(QFileDialog::Directory);
    if ( dialog.exec() ) {
        photos.clear();
        next_photo=0 ;
        QStringList files = dialog.selectedFiles();
        for ( int f = 0 ; f < files.length() ; f++ ) {
            QString file = files[f] ;
            if ( QFileInfo(file).isDir() ) {
                QDir dir(file);
                updatePhotos(dir,photos);
            } else {
                photos << file ;
            }
        }
        showPhoto();
    }
#endif

#if 0
    QFileDialog dialog;
    dialog.setFileMode(QFileDialog::Directory); //  | QFileDialog::ExistingFile);
    dialog.setOption(QFileDialog::ShowDirsOnly);
    if ( dialog.exec() ) {
        photos.clear();
        next_photo=0 ;
        QDir dir = dialog.directory();
        updatePhotos(dir,photos);
        showPhoto();
    }
#endif
}

void MainWindow::actionSave()
{
    actionSaveAs() ;
}

void MainWindow::actionSaveAs()
{
//  QString fileName =
    QFileDialog::getSaveFileName(this);
#if 0
    if (fileName.isEmpty())
        return false;

    return saveFile(fileName);
#endif
}

void MainWindow::actionPrint()         { notImplementedYet("Print"        ); }
void MainWindow::actionCut()           { notImplementedYet("Cut"          ); }
void MainWindow::actionCopy()          { notImplementedYet("Copy"         ); }
void MainWindow::actionDelete()        { notImplementedYet("Delete"       ); }
void MainWindow::actionPaste()         { notImplementedYet("Paste"        ); }
void MainWindow::actionPreferences()   { notImplementedYet("Preferences"  ); }

void MainWindow::actionFullScreen()
{
#if defined __APPLE__
    // FullScreen works on the Mac, however showNormal hangs afte a showFullScreen
    static bool bFull = false;
    if ( bFull )
        showNormal();
    else
        this->showMaximized();
    bFull = !bFull ;
#else
    if ( isFullScreen() )
        showNormal();
    else
        showFullScreen() ;
#endif
}

void MainWindow::actionOnScreenTools()
{
    if ( toolBar->isHidden() )
        toolBar->show();
    else
        toolBar->hide();
}

void MainWindow::actionExit()
{
    term();
    close() ;
}

void MainWindow::actionBack()
{
}

void MainWindow::actionForward()
{
}

void MainWindow::actionHelp()
{
    command("help");
}

void MainWindow::actionPrev()
{
    next_photo-=2;
    showPhoto();

}

void MainWindow::actionNext()
{
    showPhoto();
}

void MainWindow::actionFirst()
{
    next_photo=0;
    showPhoto();

}

void MainWindow::actionLast()
{
    next_photo=photos.length()-1;
    showPhoto();
}

static void removeNulls(std::string& s)
{
    size_t n ;
    while ( (n=s.find_first_of((char)0)) != std::string::npos )
        s.replace(n,1,"");
}

static void exifprint(const char* filename)
{
    Printf("exifprint filename = %s\n",filename);
    try {
        Exiv2::Image::AutoPtr image = Exiv2::ImageFactory::open(filename);
        assert(image.get() != 0);
        image->readMetadata();

        Exiv2::ExifData &exifData = image->exifData();
        if (exifData.empty()) {
            std::string error(filename);
            error += ": No Exif data found in the file";
            throw Exiv2::Error(1, error);
        }

        std::ostringstream oss (std::ostringstream::out);

        Exiv2::ExifData::const_iterator end = exifData.end();
        for (Exiv2::ExifData::const_iterator i = exifData.begin(); i != end; ++i) {
            const char* tn = i->typeName();

            oss << std::setw(44) << std::setfill(' ') << std::left
                      << i->key() << " "
                      << "0x" << std::setw(4) << std::setfill('0') << std::right
                      << std::hex << i->tag() << " "
                      << std::setw(9) << std::setfill(' ') << std::left
                      << (tn ? tn : "Unknown") << " "
                      << std::dec << std::setw(3)
                      << std::setfill(' ') << std::right
                      << i->count() << "  "
                      << std::dec << i->value()
                      << "\n";
        }
        oss << std::endl;
        oss << "That's all Folks in " << filename << std::endl;
        oss.flush();
        std::string output = oss.str();
        removeNulls(output);
        ::Printf("%s\n",output.c_str());
    } catch ( ... ) {
        Printf("*** unable to open filename = %s\n",filename);
    }
}

static void contentsCleaner(QString& contents)
{
    // strip off #! line at top of code
    if (contents.startsWith("#!")) {
        contents.remove(0, contents.indexOf("\n"));
    }
}

void MainWindow::runScript(const QString& fileName, bool debug)
{
    if (  m_debugger ) return ;

    if ( !m_engine   ) {
        m_engine = new QScriptEngine();
        if ( m_engine ) {
            // add File, _ and __
            File::registerClass(m_engine,"File");
            debugRegisterShort (m_engine,"_");
            debugRegisterLong  (m_engine,"__");
            debugRegisterAlert (m_engine,"alert");
            debugRegisterGlobal(m_engine,"$$");
            debugRegisterSystem(m_engine,"system");
        }
    }
    if ( !m_engine ) Q_UNUSED(exifprint("")) ;
    if ( !m_engine ) return ;

    QFile file(fileName);
    file.open(QIODevice::ReadOnly);
    QString contents = file.readAll();
    file.close();
    contentsCleaner(contents);
//    alert(contents);

#ifndef QT_NO_SCRIPTTOOLS
    if (debug) {
        if (!m_debugger) m_debugger = new QScriptEngineDebugger(this);
        if ( m_debugger ) {
            m_debugWindow = m_debugger->standardWindow();
            m_debugWindow->setWindowModality(Qt::ApplicationModal);
            m_debugWindow->resize(1280, 704);
            m_debugger->attachTo(m_engine);
            m_debugger->action(QScriptEngineDebugger::InterruptAction)->trigger();
        }
    } else {
        if (m_debugger)
            m_debugger->detach();
    }
#else
    Q_UNUSED(debug);
#endif
    // QScriptValue ret =
        m_engine->evaluate(contents, fileName);

#ifndef QT_NO_SCRIPTTOOLS
    if (m_debugWindow)
        m_debugWindow->hide();
#endif
    delete m_debugger;
    m_debugger = NULL;

    // if (ret.isError())
        // reportScriptError(ret);
}

void MainWindow::actionDebug()
{
    ::Println("actionDebug");
    // QDir::separator() == '\\' on Windows and I want / to be consistent with QDir
    //QString test = QString("%1/%2").arg(QDir::homePath()).arg("test.js");
    //::Printf("test = %s\n",SS(test));

    runScript(QString(":/Resources/mkalbum.qs"),true);
/*
    QString message = QString("appDirPath = %1\n"
                              "test.js = %2\n"
                             ).arg(theApp->applicationDirPath())
                              .arg(test)
                             ;

    alert(message);
*/
    // exifprint("R.jpg");
    // exifprint("C:/Robin/Projects/cocoatutorials/QtSee-build-desktop/R.jpg");

/*
    QString in("/Users/rmills/R.jpg");
    QString out("/Users/rmills/R_scaled.jpg");

    Println("about to open image");
    QImage image(in);
    Printf("image size = %d,%d\n",image.width(),image.height());
    int w = image.width()/4;
    int h = image.height()/4;
    Println("about to scale image");
    image.scaled(QSize(w,h));
    Println("about to save image");
    image.save(out);
    Println("done");
*/
}

void MainWindow::actionRun()
{
    runScript(QString(":/Resources/mkalbum.qs"),false);
}

void MainWindow::actionReload()
{
    view->pageAction(QWebPage::Reload);
}

void MainWindow::actionStop()
{
    view->pageAction(QWebPage::Stop);
}

void MainWindow::command(QString c)
{
    locationEdit->setText(c);
    changeLocation();
}

void MainWindow::actionAbout()
{
    about*  dialog = new about;
    dialog->setWindowFlags(Qt::Tool);
    dialog->move( int(this->x() + 0.5 * this->width()  - 0.5 * dialog->width())
                , int(this->y() + 0.5 * this->height() - 0.5 * dialog->height())
                );

    dialog->setWindowIcon(QIcon(":/icon/ikona_32.png"));
    dialog->setWindowModality(Qt::ApplicationModal);
    dialog->show();
    dialog->activateWindow();
}

void MainWindow::viewSource()
{
    QNetworkAccessManager* accessManager = view->page()->networkAccessManager();
    QNetworkRequest request(view->url());
    QNetworkReply* reply = accessManager->get(request);
    connect(reply, SIGNAL(finished()), this, SLOT(slotSourceDownloaded()));
}

void MainWindow::slotSourceDownloaded()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(const_cast<QObject*>(sender()));
    QTextEdit* textEdit = new QTextEdit(NULL);
    textEdit->setAttribute(Qt::WA_DeleteOnClose);
    textEdit->show();
    textEdit->setPlainText(reply->readAll());
    reply->deleteLater();
}

void MainWindow::adjustLocation()
{
    locationEdit->setText(view->url().toString());
}

void MainWindow::showPhoto()
{
    static const int maxFiles = 18;
    QString          location;

    // copy resource photos to the temporary directory
    if ( ! tempFiles.size() ) {
        for ( int p = 1 ; p < maxFiles ; p++ ) {
            QFile file(QString(":/Resources/p%1.jpg").arg(p));
            QTemporaryFile* pTemp  = QTemporaryFile::createLocalFile(file);
            QString newName=QString("%1.jpg").arg(pTemp->fileName());
            pTemp->rename(newName);
            tempFiles.insert(tempFiles.end(),newName);
        }
    }

    // populate the photos with the tempFiles if necessary
    if ( ! photos.length() ) {
        for ( int p = 0 ; p < (int) tempFiles.size() ; p++ )
            photos << tempFiles[p];
    }

    // display the next photo
    if ( next_photo >= photos.size() || next_photo < 0 )
        next_photo = 0 ;
    QString photo;
    if ( photos.size()) {
         photo    = photos[next_photo++];
         location = QString("file:///%1").arg(photo);
         location.replace("\\","/");
         location.replace("////","///");
    }
//  alert (location);

    // stamp a web page using the template in the resources
    QFile file(":/Resources/photo.html");
    file.open(QIODevice::ReadOnly);
    QString html  = file.readAll();
    file.close();

    // update and display the template
    html.replace("__LOCATION__",location);
    html.replace("__TITLE__",photo);
    view->page()->mainFrame()->setContent(SS(html));
    locationEdit->setText(location);
}

QString MainWindow::command_help(QString& /*c*/)
{
    showPhoto();
    return "";
}

QString MainWindow::command_run(QString& c)
{
    QString location = c;
    return location;
}

void MainWindow::changeLocation()
{
    QString location = locationEdit->text();

    // if the user entered "help", show a photo from the resources
    if ( location == "help" ) location = command_help(location);
    if ( location == "run"  ) location = command_run(location);

    if ( location.length()) {
        QUrl url = QUrl(location);
        view->load(url);
        view->setFocus();
    }
}

void MainWindow::adjustTitle()
{
    if (progress <= 0 || progress >= 100)
        setWindowTitle(view->title());
    else
        setWindowTitle(QString("%1 (%2%)").arg(view->title()).arg(progress));
}

void MainWindow::setProgress(int p)
{
    progress = p;
    adjustTitle();
}

void MainWindow::finishLoading(bool)
{
    progress = 100;
    adjustTitle();
    view->page()->mainFrame()->evaluateJavaScript(jQuery);

    rotateImages(rotateAction->isChecked());
}

void MainWindow::highlightAllLinks()
{
    QString code = "$('a').each( function () { $(this).css('background-color', 'yellow') } )";
    view->page()->mainFrame()->evaluateJavaScript(code);
}

void MainWindow::rotateImages(bool invert)
{
    QString code;
    if (invert)
        code = "$('img').each( function () { $(this).css('-webkit-transition', '-webkit-transform 2s'); $(this).css('-webkit-transform', 'rotate(180deg)') } )";
    else
        code = "$('img').each( function () { $(this).css('-webkit-transition', '-webkit-transform 2s'); $(this).css('-webkit-transform', 'rotate(0deg)') } )";
    view->page()->mainFrame()->evaluateJavaScript(code);
}

void MainWindow::removeGifImages()
{
    QString code = "$('[src*=gif]').remove()";
    view->page()->mainFrame()->evaluateJavaScript(code);
}

void MainWindow::removeInlineFrames()
{
    QString code = "$('iframe').remove()";
    view->page()->mainFrame()->evaluateJavaScript(code);
}

void MainWindow::removeObjectElements()
{
    QString code = "$('object').remove()";
    view->page()->mainFrame()->evaluateJavaScript(code);
}

void MainWindow::removeEmbeddedElements()
{
    QString code = "$('embed').remove()";
    view->page()->mainFrame()->evaluateJavaScript(code);
}

// That's all Folks!
////
