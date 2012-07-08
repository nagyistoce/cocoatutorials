@echo off
if defined VCINSTALLDIR ( 
	echo already installed
	goto eof
)
call "C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\bin\amd64\vcvars64.bat" 
set INCLUDE=%INCLUDE%;c:\home\rmills\dev\win32\boost\include\boost-1_42
set BOOST_ROOT=c:\home\rmills\dev\win32\boost\include\boost-1_42

:eof
