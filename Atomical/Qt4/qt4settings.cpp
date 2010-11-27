#include "qt4settings.h"
#include <QColor>

Qt4Settings::Qt4Settings()
: QSettings(QSettings::IniFormat,QSettings::UserScope,QString("clanmills.com"),QString("Atomical"))
{
    sFullScreenControls = "global/fullScreenControls";
    sFullScreenMenubar  = "global/fullScreenMenubar";
    sNativeDialogs      = "global/nativeDialogs";

    sBackgroundRed      = "background/red";
    sBackgroundGreen    = "background/green";
    sBackgroundBlue     = "background/blue";

    vTrue=true;
    vFalse=false;
}

QColor Qt4Settings::backgroundColor()
{
    int r = value(sBackgroundRed,0).toInt();
    int g = value(sBackgroundGreen,255).toInt();
    int b = value(sBackgroundBlue,170).toInt();
    return QColor(r,g,b);
}

// That's all Folks!
////
