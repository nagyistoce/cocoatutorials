@echo off

rem General environmental setup script for PS development with Win NT

rem Look for "Edit:" below, which marks things that will need to be
rem changed for particular systems

if "%1" == "/?" goto USAGE

rem Edit: This should give access to all file-systems via a smb-root
set SAMBA_DRIVE=y:

rem Edit: This is the network path to set up on the SAMBA_DRIVE
set NETPATH=\\cranky\smb-root

rem Edit: This is the use-id for the SAMBA_DRIVE
set NETUSER=2psczar

rem Edit: This is the location of this script
set PSTOOLS=e:\2ps\pstools

rem setup environment for compiler

rem Edit: If you don't have VC++4 change this to ERROR
:if "%1" == "win32vc4" goto ERROR
if "%1" == "win32vc4" goto CASE5

rem Edit: If you don't have VC++5 change this to ERROR
if "%1" == "win32vc5" goto CASE5

rem Edit: This is the default, if only have VC++ 5 change to ERROR
goto ERROR

:CASE4
rem This is the MS VC++ 4 environment
rem Edit: the path and the processor architecture
rem choose x86 / ALPHA
echo Using MSDEV x86-CASE 4 in e:\2ps\pstools\setenv.bat 
call D:\DevStudio\VC\bin\vcvars32 x86
goto CONT

:CASE5
rem This is the MS VC++ 5 environment
rem Edit: the path and the processor architecture
rem choose x86 / ALPHA
echo Using MSDEV5-CASE 5 in e:\2ps\pstools\setenv.bat 
call D:\DevStudio\VC\bin\vcvars32 x86
set MSDevDir=D:\DevStudio\VC
goto CONT

:ERROR
rem This is for missing cases; goal: to ensure that compiles will fail
echo ERROR: Compiler version not installed
set PATH=C:\perl5\bin;C:\WINNT35\system32;C:\WINNT35
set MSDevDir=
set MSVCDir=
goto FIN

:CONT
if "%ACMEDIR%" == "" set ACMEDIR=%SAMBA_DRIVE%\usr\2ps\ACME

rem Edit: you can put additional things in the path here
rem Note: This is done after "vcvars32" so that you can override it
SET PATH=%PSTOOLS%;%ACMEDIR%\winnt\bin;%PATH%
goto FIN

:USAGE
echo "setenv [develop_env]"

:FIN
