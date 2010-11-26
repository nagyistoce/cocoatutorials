#ifndef Qt4Settings_H
#define Qt4Settings_H

#include <QApplication>
#include <QSettings>
#include <QVariant>

class Qt4Settings : public QSettings
{
    Q_OBJECT
public:
    explicit Qt4Settings();

public:
    QString    sFullScreenControls ;
    QString    sFullScreenMenubar;
    QString    sNativeDialogs;

    QString    sBackgroundRed;
    QString    sBackgroundGreen;
    QString    sBackgroundBlue;

    QVariant   vTrue;
    QVariant   vFalse;

signals:

public slots:

};

#endif // Qt4Settings_H
