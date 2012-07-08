/*       (c) Robin Mills Camberley England 1993
------------------------------------------------------------------------
* File:      diction.cpp
* Date:      12 April 1993    crap
* Author:    Robin Mills
------------------------------------------------------------------------
* Modified By Reason

------------------------------------------------------------------------
* Purpose:
  This C++ Program is DLL for doing language lookup

* Usage:
  This is a windows DLL

  To call from hDC --> diction.dll robin(1)

* Return value:
  None.

* Example:
  None
                                         
* Description:
  None.

* Restrictions and Side Effects:
  ******************************************************
  ***                                                ***
  ***  This must be compiled with a Borland Product  ***
  ***                                                ***
  ******************************************************
------------------------------------------------------------------------
*/
// Modified from code supplied by Borland
// Borland C++ - (C) Copyright 1991, 1992 by Borland International

// Example program used to demonstrate DLL's. This file is one of the
// files used to build BITMAP.DLL which is used in the DLLDEMO program.

/*-------------------------------------------------------*/
/* Include files                                         */
/*-------------------------------------------------------*/
#define  STRICT
# include <owl.h>
# include <bwcc.h>
# include <listbox.h>
# include <combobox.h>    //TComboBox

# include <stdio.h>
# include <time.h>
# include <dos.h>

# include "diction.h"

/*-------------------------------------------------------*/
/* Macros and static definitions                         */
/*-------------------------------------------------------*/
# define min(a,b) a < b ? a : b
# define max(a,b) a > b ? a : b
# define Message(x) MessageBox(NULL,x,"From diction.dll",0)
# define Compare(x,y) !lstrcmp(x,y)
PTModule pModule  ;
char szEdit   [200] ;
char szInput  [200] ;
char szMessage[200] ;
int  nResult        ;
UINT nTimer         ;

/*-------------------------------------------------------*/
/* LibMain - entry point of DLL                          */
/*-------------------------------------------------------*/
// Turn off warning: Parameter '' is never used
#pragma argsused

// Every DLL has an entry point LibMain and an exit point WEP.
// extern "C"
int FAR PASCAL LibMain
( HINSTANCE hInstance
, WORD      wDataSegment
, WORD      wHeapSize
, LPSTR lpszCmdLine
) {
    // The startup code for the DLL initializes the local heap (if there is one)
    // with a call to LocalInit which locks the data segment.
    if ( wHeapSize != 0 )
        UnlockData( 0 );

    pModule = new TModule("diction",hInstance,lpszCmdLine) ;
    return 1;   // Indicate that the DLL was initialized successfully.
}

/*-------------------------------------------------------*/
/* Wep - called when DLL is unloaded                     */
/*-------------------------------------------------------*/
// Turn off warning: Parameter '' is never used
#pragma argsused

int FAR PASCAL WEP ( int bSystemExit )
{
    return 1;
}

/*-------------------------------------------------------*/
/* Centre - show a window centred on the cursor          */
/*-------------------------------------------------------*/
void Centre
( HWND hWnd
) {
    RECT  rect  ;
    POINT point ;
    GetWindowRect(hWnd,&rect ) ;
    GetCursorPos (     &point) ;
    point.x -= ( rect.right  - rect.left) / 2 ; point.x = max(0,point.x) ;
    point.y -= ( rect.bottom - rect.top ) / 2 ; point.y = max(0,point.y) ;

    SetWindowPos(hWnd,NULL,point.x,point.y,0,0,SWP_NOZORDER|SWP_NOSIZE) ;
}

/*=================================================================*/
/* TOptionsDialog = class definition for the Options Dialog Box    */
/*=================================================================*/
class TOptionsDialog : public TDialog
{
  public:
    PTComboBox pLanguages   ;
    TOptionsDialog( void )  ;

  virtual void WMInitDialog (RTMessage Msg) = [WM_FIRST + WM_INITDIALOG      ] ;
  virtual void WMDestroy    (RTMessage Msg) = [WM_FIRST + WM_DESTROY         ] ;
  virtual void WMTimer      (RTMessage Msg) = [WM_FIRST + WM_TIMER           ] ;
  virtual void Help         (RTMessage Msg) = [ID_FIRST + IDHELP             ] ;
  virtual void Top          (RTMessage Msg) = [ID_FIRST + IDD_OptionTop      ] ;
} ;

/*-----------------------------------------------------------------*/
/* TOptionsDialog::TOptionsDialog                                  */
/*-----------------------------------------------------------------*/
TOptionsDialog::TOptionsDialog( void )
: TDialog(NULL , IDD_DictionOptions,pModule)
{
} ;

/*-----------------------------------------------------------------*/
/* TOptionsDialog::WMDestroy                                       */
/*-----------------------------------------------------------------*/
void TOptionsDialog::WMDestroy( RTMessage )
{
    KillTimer(HWindow,nTimer) ;
}

/*-----------------------------------------------------------------*/
/* TOptionsDialog::Help                                            */
/*-----------------------------------------------------------------*/
void TOptionsDialog::Help( RTMessage )
{
    WinExec("winhelp.exe winfile",SW_SHOW) ;
}

/*-----------------------------------------------------------------*/
/* TOptionsDialog::Top                                               */
/*-----------------------------------------------------------------*/
void TOptionsDialog::Top( RTMessage )
{
    HWND hWndState = IsDlgButtonChecked(HWindow,IDD_OptionTop) ? HWND_TOPMOST : HWND_NOTOPMOST ;
    SetWindowPos(HWindow,hWndState,0,0,0,0,SWP_NOMOVE | SWP_NOSIZE) ;
}

/*-----------------------------------------------------------------*/
/* TOptionsDialog::WMInitDialog                                    */
/*-----------------------------------------------------------------*/
void TOptionsDialog::WMInitDialog(RTMessage )
{
    SetDlgItemText(HWindow,IDD_DictionDate,__DATE__) ;
    SetDlgItemText(HWindow,IDD_DictionTime,__TIME__) ;
    nTimer = SetTimer(HWindow,1,1000,NULL) ;
    SendMessage(HWindow,WM_TIMER,0,0L) ;
    HICON hIcon = LoadIcon( pModule->hInstance, MAKEINTRESOURCE(IDD_Diction) ) ;
    SetClassWord(HWindow,GCW_HICON,(UINT) hIcon) ;
    SendDlgItemMsg(IDD_OptionLanguage, CB_ADDSTRING, 0, (LONG)"English-English"   );
    SendDlgItemMsg(IDD_OptionLanguage, CB_ADDSTRING, 0, (LONG)"English-French"    );
    SendDlgItemMsg(IDD_OptionLanguage, CB_ADDSTRING, 0, (LONG)"French-English"    );
    SendDlgItemMsg(IDD_OptionLanguage, CB_ADDSTRING, 0, (LONG)"French-French"     );
    SendDlgItemMsg(IDD_OptionLanguage, CB_ADDSTRING, 0, (LONG)"Serbo-Croat"       );
    SendDlgItemMsg(IDD_OptionLanguage, CB_ADDSTRING, 0, (LONG)"Mongolian-Chinese" );
    SendDlgItemMsg(IDD_OptionLanguage, CB_ADDSTRING, 0, (LONG)"Australian-English");

    SendDlgItemMsg(IDD_OptionLanguage, CB_SETCURSEL, 0, 0L);

    Centre(HWindow) ;
}
/*-----------------------------------------------------------------*/
/* TOptionsDialog::WMInitDialog                                    */
/*-----------------------------------------------------------------*/
void TOptionsDialog::WMTimer(RTMessage )
{
    struct tm *time_now;
    time_t     secs_now;
    char       szTime[80];
    tzset();
    time(&secs_now);
    time_now = localtime(&secs_now);
    strftime(szTime, sizeof szTime, "%H:%M:%S", time_now);
    SetDlgItemText(HWindow,IDD_DictionTime,szTime) ;
}

/*=================================================================*/
/* TAboutDialog - class definition for the About Dialog Box        */
/*=================================================================*/
class TAboutDialog : public TDialog
{
  public:
    PTListBox pMenu ;

  TAboutDialog( void ) ;
  ~TAboutDialog( ) ;

  virtual void WMInitDialog (RTMessage Msg) = [WM_FIRST + WM_INITDIALOG      ] ;
  virtual void Help         (RTMessage Msg) = [ID_FIRST + IDHELP             ] ;
  virtual void Options      (RTMessage Msg) = [ID_FIRST + IDD_DictionOptions ] ;
  virtual void WMDestroy    (RTMessage Msg) = [WM_FIRST + WM_DESTROY         ] ;
} ;
/*-----------------------------------------------------------------*/
/* TAboutDialog::TAboutDialog                                      */
/*-----------------------------------------------------------------*/
TAboutDialog::TAboutDialog( void )
: TDialog(NULL , IDD_DictionBox,pModule) {
    pMenu = new TListBox(this,IDD_DictionMenu,pModule) ;
} ;

/*-----------------------------------------------------------------*/
/* TAboutDialog::~TAboutDialog                                      */
/*-----------------------------------------------------------------*/
TAboutDialog::~TAboutDialog( void )
{
    delete pMenu ;
} ;

/*-----------------------------------------------------------------*/
/* TAboutDialog::WMDestroy                                         */
/*-----------------------------------------------------------------*/
void TAboutDialog::WMDestroy( RTMessage )
{
    GetDlgItemText(HWindow,IDD_DictionEdit,szEdit,sizeof szEdit) ;
}

/*-----------------------------------------------------------------*/
/* TAboutDialog::Help                                              */
/*-----------------------------------------------------------------*/
void TAboutDialog::Help( RTMessage )
{
    WinExec("winhelp.exe progman",SW_SHOW) ;
}

/*-----------------------------------------------------------------*/
/* TAboutDialog::Options                                           */
/*-----------------------------------------------------------------*/
void TAboutDialog::Options( RTMessage )
{
    nResult = pModule->ExecDialog( new TOptionsDialog() ) ;
}

/*-----------------------------------------------------------------*/
/* TAboutDialog::WMInitDialog                                      */
/*-----------------------------------------------------------------*/
void TAboutDialog::WMInitDialog(RTMessage )
{
    UINT uFormat = CF_TEXT ;
    HWND hWnd    = GetActiveWindow() ;
    SetDlgItemText(HWindow,IDD_DictionTime,__TIME__) ;
    SetDlgItemText(HWindow,IDD_DictionDate,__DATE__) ;
    if ( OpenClipboard(hWnd) ) {
      if ( IsClipboardFormatAvailable(uFormat) ) {
        HGLOBAL hText  =         GetClipboardData(uFormat) ;
        LPSTR   szText = (LPSTR) GlobalLock      (hText  ) ;
        if ( szText ) {
          int length = min(lstrlen(szText),sizeof szInput -1) ;
          lstrcpyn(szInput,szText,length+1)                ;
          SetDlgItemText(HWindow,IDD_DictionText,szText) ;
          SetDlgItemText(HWindow,IDD_DictionEdit,szText) ;
        }
        GlobalUnlock(hText);
      }
      CloseClipboard() ;
    }

    SendDlgItemMsg(IDD_DictionMenu, LB_ADDSTRING, 0, (LONG)"Item 1");
    SendDlgItemMsg(IDD_DictionMenu, LB_ADDSTRING, 0, (LONG)"Item 2");
    SendDlgItemMsg(IDD_DictionMenu, LB_ADDSTRING, 0, (LONG)"Item 3");
    SendDlgItemMsg(IDD_DictionMenu, LB_ADDSTRING, 0, (LONG)"Item 4");
    SendDlgItemMsg(IDD_DictionMenu, LB_ADDSTRING, 0, (LONG)"Item 5");
    SendDlgItemMsg(IDD_DictionMenu, LB_ADDSTRING, 0, (LONG)"Item 6");
    SendDlgItemMsg(IDD_DictionMenu, LB_ADDSTRING, 0, (LONG)"Item 7");

    SendDlgItemMsg(IDD_DictionMenu, LB_SETCURSEL, 0, 0L);

    Centre(HWindow) ;
}


/*-------------------------------------------------------*/
/* Robin - external function of DLL                      */
/*-------------------------------------------------------*/
#pragma argsused
extern "C" void FAR PASCAL _export Robin
( int n
) {
    HWND hWnd  = GetFocus() ;
    szInput[0] = '\0' ;
    int nResult = pModule->ExecDialog( new TAboutDialog() ) ;
    if ( nResult == IDOK && szInput[0] && !Compare(szInput,szEdit) )
      for ( int i = 0 ; i < lstrlen(szEdit) ; i++ ) {
        SendMessage(hWnd,WM_KEYDOWN,szEdit[i],0L) ;
        SendMessage(hWnd,WM_CHAR   ,szEdit[i],0L) ;
        SendMessage(hWnd,WM_KEYUP  ,szEdit[i],0L) ;
      }
}

/*-------------------------------------------------------*/
/* Alan - external function of DLL                       */
/*-------------------------------------------------------*/
#pragma argsused
extern "C" void FAR PASCAL _export Alan
( int n
) {
    char szMessage[60] ;
    HWND hWnd = GetActiveWindow() ;
    GetWindowText(hWnd,szMessage,sizeof szMessage) ;
    // sprintf(szMessage,"Hello - %d",n) ;
    MessageBox(NULL,szMessage,"Alan",0) ;
}

