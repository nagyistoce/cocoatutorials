#include "qt4settings.h"

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
