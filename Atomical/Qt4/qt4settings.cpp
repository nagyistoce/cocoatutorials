#include "qt4settings.h"

Qt4Settings::Qt4Settings()
{
    settings = new QSettings(QSettings::IniFormat,QSettings::UserScope,QString("clanmills.com"),QString("Atomical"));

    sFullScreenControls = "general/fullScreenControls";
    sFullScreenMenubar  = "general/fullScreenMenubar";
    sNativeControls     = "general/nativeControls";

    sBackgroundRed      = "background/red";
    sBackgroundGreen    = "background/green";
    sBackgroundBlue     = "background/blue";

    vTrue=true;
    vFalse=false;
}

Qt4Settings::~Qt4Settings()
{
    settings->sync();
    delete settings ;
}

