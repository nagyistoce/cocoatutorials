/********************************************************************************
** Form generated from reading ui file 'about.ui'
**
** Created: Thu Mar 19 06:23:21 2009
**      by: Qt User Interface Compiler version 4.4.3
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_ABOUT_H
#define UI_ABOUT_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QLabel>

QT_BEGIN_NAMESPACE

class Ui_about
{
public:
    QLabel *label;

    void setupUi(QDialog *about)
    {
    if (about->objectName().isEmpty())
        about->setObjectName(QString::fromUtf8("about"));
    about->setWindowModality(Qt::ApplicationModal);
    about->resize(396, 264);
    about->setMinimumSize(QSize(396, 264));
    about->setMaximumSize(QSize(396, 264));
    about->setContextMenuPolicy(Qt::NoContextMenu);
    QIcon icon;
    icon.addPixmap(QPixmap(QString::fromUtf8(":/icon/icon")), QIcon::Normal, QIcon::Off);
    about->setWindowIcon(icon);
    about->setModal(true);
    label = new QLabel(about);
    label->setObjectName(QString::fromUtf8("label"));
    label->setGeometry(QRect(10, 10, 371, 251));
    label->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
    label->setWordWrap(true);
    label->setTextInteractionFlags(Qt::LinksAccessibleByKeyboard|Qt::LinksAccessibleByMouse);

    retranslateUi(about);

    QMetaObject::connectSlotsByName(about);
    } // setupUi

    void retranslateUi(QDialog *about)
    {
    about->setWindowTitle(QApplication::translate("about", "About", 0, QApplication::UnicodeUTF8));
    label->setText(QApplication::translate("about", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Sans'; font-size:10pt; font-weight:400; font-style:normal;\">\n"
"<p align=\"justify\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"> This application is an example of using QTCreator/QTDesigner for GUI design. It shows how to place an qGLWidget into a form with use of qtDesigner module.</p>\n"
"<p align=\"justify\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"> Some GUI elements was created 'by handy' to ilustrate proces. Mostly based on code found in 'QTExamples' folder.</p>\n"
"<p align=\"justify\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-inde"
        "nt:0px;\"> My work was to figure out how to place that damn qGLWidget in a place that it should be, and manipulate it. I know that what i did is not a perfect solution but ... its do the job.</p>\n"
"<p align=\"justify\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"> </p>\n"
"<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Author: <a href=\"http://lugru.com\"><span style=\" text-decoration: underline; color:#0000ff;\">lugru</span></a></p></body></html>", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(about);
    } // retranslateUi

};

namespace Ui {
    class about: public Ui_about {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ABOUT_H
