/*
  File:    Utilities.c

  Contains:  General utility routines

  Written by: Chris White  

  Copyright:  Copyright � 1995-1999 by Apple Computer, Inc., All Rights Reserved.

        You may incorporate this Apple sample source code into your program(s) without
        restriction. This Apple sample source code has been provided "AS IS" and the
        responsibility for its operation is yours. You are not permitted to redistribute
        this Apple sample source code as "Apple sample source code" after having made
        changes. If you're going to re-distribute the source, we require that you make
        it clear in the source that the code was descended from Apple sample source
        code, but that you've made changes.

  Change History (most recent first):
        8/5/1999  Karl Groethe  Updated for Metrowerks Codewarror Pro 2.1
        

*/


#ifndef __TYPES__
  #include <Types.h>
#endif

#ifndef __TEXTUTILS__
  #include <TextUtils.h>
#endif

#ifndef __DIALOGS__
  #include <Dialogs.h>
#endif


#ifndef __CODEFRAGMENTS__
  #include <CodeFragments.h>
#endif

#include <Folders.h>
#include <Resources.h>





#include "FragmentTool.h"
#include "Utilities.h"

#include "ProtoTypes.h"



//
// Tell the user that something went wrong
//
void AlertUser ( short messageCode, short errorNum, StringPtr theString )
{
  Str255    messageString;
  Str255    numberString;
  
  if ( messageCode > 0 )
    GetIndString ( messageString, rErrorStrings, messageCode );
  else
    messageString[0] = '```SourceCodeText';
   
  if ( errorNum != 0 )
    NumToString ( errorNum, numberString );
  else
    numberString[0] = '```SourceCodeText';
   
  ParamText ( messageString, numberString, theString, (StringPtr) "\p" );
  
  StopAlert ( kErrorDialog, nil );
  
  // We need to clear the param text so it isn't used by mistake
  ParamText ( (StringPtr) "\p", (StringPtr) "\p", (StringPtr) "\p", (StringPtr) "\p" );
  
  return;
}



pascal void OutlineUserItem ( DialogRef theDialog, short theItem )
{
  short  theType;
  Handle  theHan;
  Rect  theRect;
  
  GetDialogItem ( theDialog, theItem, &theType, &theHan, &theRect );
  PenNormal ( );
  InsetRect ( &theRect, -1, -1 );
  FrameRect ( &theRect );
  
  return;
}



StringPtr CopyPStr ( Str255  inSourceStr, StringPtr outDestStr, int16 inDestSize )
{
  int8 dataLen = inSourceStr[0] + 1;
  if ( dataLen > inDestSize )
    dataLen = inDestSize;
  
  BlockMoveData ( inSourceStr, outDestStr, dataLen );
  outDestStr[0] = dataLen - 1;
  return outDestStr;
}



StringPtr ConcatPStr ( Str255 ioFirstStr, Str255 inSecondStr, int16 inDestSize )
{
    int8 charsToCopy = inSecondStr[0];
    
    if ( ioFirstStr[0] + charsToCopy > inDestSize - 1 )
      charsToCopy = inDestSize - 1 - ioFirstStr[0];

    BlockMoveData ( inSecondStr + 1,  ioFirstStr + ioFirstStr[0] + 1, charsToCopy );
    ioFirstStr[0] += charsToCopy;
    
    return ioFirstStr;
}



StringPtr OSTypeToPStr ( OSType inOSType, StringPtr outString )
{
  BlockMoveData ( &inOSType, outString + 1, sizeof ( OSType ) );
  outString[0] = sizeof ( OSType );

  return outString;
}



void BlockClear ( Ptr ptr, char value, Size size )
{
  int i;
  
  for ( i = 0; i < size; i++ )
    *ptr++ = value;
  
  return;
}



Boolean IsAResource ( Handle theHan )
{
  const short kResNotFound = -1;
  return HomeResFile ( theHan ) != kResNotFound;
}



OSErr CreateTemporaryFile ( FSSpecPtr theSpec )
{
  OSErr    theErr = noErr;
  int16    theVol;
  int32    theDir;
  Str255    tmpStr = "\p";
  Str255    theName = "\pFragmentTool ";
  
  
  
  theErr = FindFolder ( kOnSystemDisk, kTemporaryFolderType, kCreateFolder, &theVol, &theDir );
  if ( theErr ) goto CleanupAndBail;
  
  OSTypeToPStr ( TickCount ( ), tmpStr );
  ConcatPStr ( theName, tmpStr, sizeof ( Str255 ) );
  theErr = FSMakeFSSpec ( theVol, theDir, theName, theSpec );
  if ( theErr != fnfErr )
  {
    theErr = (theErr == fnfErr) ? dupFNErr : theErr;
    goto CleanupAndBail;
  }
  
  theErr = FSpCreate ( theSpec, kFourQuestionMarks, kCFragLibraryFileType, smSystemScript );
  if ( theErr ) goto CleanupAndBail;
  
  // The file has been created with both forks, but it doesn't have a
  // resource map yet. If we try to open it now, we'll get an eofErr (-39).
  FSpCreateResFile ( theSpec, kFourQuestionMarks, kCFragLibraryFileType, smSystemScript );
  
  
  // goto's are okay for error handling
CleanupAndBail:
  return theErr;
}



#if DEBUGGING
void DebugStrNum ( Str255 str, long num )
{
  Str255 debug_str, tmp_str;
  
  BlockMoveData ( &str[0], &debug_str[0], str[0] + 1 );
  
  NumToString ( num , &tmp_str[0] );
  debug_str[debug_str[0] + 1] = ' ';
  BlockMoveData ( &tmp_str[1], &debug_str[debug_str[0] + 2], tmp_str[0] );
  debug_str[0] = debug_str[0] + tmp_str[0] + 1;
  DebugStr ( debug_str );
  
  return;
}
#endif



