@echo off
if defined VCINSTALLDIR ( 
	echo already installed
	goto eof
)
call "C:\Program Files (x86)\Microsoft Visual Studio 9.0\VC\bin\vcvars32.bat"
set INCLUDE=%INCLUDE%;%BOOST_ROOT%;c:\Program Files (x86)\GnuWin32\include
set LIB=%LIB%;C:\Program Files (x86)\GnuWin32\lib

:eof
