#ifndef Qt4Settings_H
#define Qt4Settings_H

#include <QApplication>
#include <QSettings>
#include <QVariant>

class Qt4Settings : public QObject
{
    Q_OBJECT
public:
    explicit Qt4Settings();
    virtual ~Qt4Settings();

public:
    QSettings* settings ;
    QString    sFullScreenControls ;
    QString    sFullScreenMenubar;
    QString    sNativeControls;

    QString    sBackgroundRed;
    QString    sBackgroundGreen;
    QString    sBackgroundBlue;

    QVariant   vTrue;
    QVariant   vFalse;

signals:

public slots:

};

#endif // Qt4Settings_H
