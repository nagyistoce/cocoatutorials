@rem = '--*-PERL-*--';
@rem = '
@echo off
rem setlocal
set ARGS=
set PROGRAM=%0
:loop
if .%1==. goto endloop
set ARGS=%ARGS% %1
shift
goto loop
:endloop
rem ***** This assumes PERL is in the PATH *****
perl -w -S %PROGRAM% %ARGS%
