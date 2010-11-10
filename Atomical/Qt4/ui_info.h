/********************************************************************************
** Form generated from reading ui file 'info.ui'
**
** Created: Thu Mar 19 03:39:30 2009
**      by: Qt User Interface Compiler version 4.4.3
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_INFO_H
#define UI_INFO_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>

QT_BEGIN_NAMESPACE

class Ui_info
{
public:

    void setupUi(QDialog *info)
    {
    if (info->objectName().isEmpty())
        info->setObjectName(QString::fromUtf8("info"));
    info->resize(320, 240);

    retranslateUi(info);

    QMetaObject::connectSlotsByName(info);
    } // setupUi

    void retranslateUi(QDialog *info)
    {
    info->setWindowTitle(QApplication::translate("info", "Dialog", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(info);
    } // retranslateUi

};

namespace Ui {
    class info: public Ui_info {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_INFO_H
