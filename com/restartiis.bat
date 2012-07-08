@echo off
cls
echo RESTART - A restart utility for IIS web services.
echo June 1998, Microsoft Corporation.
echo ****************************************>>%SystemRoot%\restart.log
echo Stop Date/Time:>>%SystemRoot%\restart.log
echo. | date | find /i "current">>%SystemRoot%\restart.log
echo. | time | find /i "current">>%SystemRoot%\restart.log
echo.>>%SystemRoot%\restart.log
echo Stopping Web Services...
echo.

set MSFTPSVC=0
set NNTPSVC=0
set SMTPSVC=0
set W3SVC=0
set BROKSVC=0
set LDAPSVC=0
set MSGBLDSVC=0
set IISERROR=0

:MSFTPSVC
net start | find /i "FTP Publishing Service">NUL
if errorlevel==1 goto NNTPSVC
set MSFTPSVC=1

:NNTPSVC
net start | find /i "Microsoft NNTP Service">NUL
if errorlevel==1 goto SMTPSVC
set NNTPSVC=1

:SMTPSVC
net start | find /i "Microsoft SMTP Service">NUL
if errorlevel==1 goto W3SVC
set SMTPSVC=1

:W3SVC
net start | find /i "World Wide Web Publishing Service">NUL
if errorlevel==1 goto BROKSVC
set W3SVC=1

:BROKSVC
net start | find /i "Site Server Authentication Service">NUL
if errorlevel==1 goto LDAPSVC
set BROKSVC=1

:LDAPSVC
net start | find /i "Site Server LDAP Service">NUL
if errorlevel==1 goto MSGBLDSVC
set LDAPSVC=1

:MSGBLDSVC
net start | find /i "Site Server Message Builder Service">NUL
if errorlevel==1 goto STOPIIS
set MSGBLDSVC=1

:STOPIIS
net stop iisadmin /y>>%SystemRoot%\restart.log
if errorlevel==1 goto STOPERROR
goto STOPPED

:STOPERROR
REM ********************
REM * Put any desired error-handling commands here.
REM * For example, if you have the NT Resource Kit,
REM * you could use the following command to stop
REM * IIS down the hard way:
REM *     KILL.EXE INETINFO.EXE
REM ********************
set IISERROR=1

:STOPPED
REM ********************
REM * Put any desired commands to run while IIS is stopped here.
REM * For example, if you have the Windows NT Resource Kit,
REM * you could use the following command to pause
REM * the restart for one minute:
REM *     SLEEP.EXE 60
REM ********************

:STARTIIS
echo -------------------->>%SystemRoot%\restart.log
echo Start Date/Time:>>%SystemRoot%\restart.log
echo. | date | find /i "current">>%SystemRoot%\restart.log
echo. | time | find /i "current">>%SystemRoot%\restart.log
echo.>>%SystemRoot%\restart.log
echo Starting web services...
echo.

if %W3SVC%==0 goto NOW3SVC
net start W3SVC>>%SystemRoot%\restart.log
if errorlevel==1 set IISERROR=1

:NOW3SVC
if %MSFTPSVC%==0 goto NOMSFTPSVC
net start MSFTPSVC>>%SystemRoot%\restart.log
if errorlevel==1 set IISERROR=1

:NOMSFTPSVC
if %NNTPSVC%==0 goto NONNTPSVC
net start NNTPSVC>>%SystemRoot%\restart.log
if errorlevel==1 set IISERROR=1

:NONNTPSVC
if %SMTPSVC%==0 goto NOSMTPSVC
net start SMTPSVC>>%SystemRoot%\restart.log
if errorlevel==1 set IISERROR=1

:NOSMTPSVC
if %BROKSVC%==0 goto NOBROKSVC
net start BROKSVC>>%SystemRoot%\restart.log
if errorlevel==1 set IISERROR=1

:NOBROKSVC
if %LDAPSVC%==0 goto NOLDAPSVC
net start LDAPSVC>>%SystemRoot%\restart.log
if errorlevel==1 set IISERROR=1

:NOLDAPSVC
if %LDAPSVC%==0 goto NOMSGBLDSVC
net start MSGBLDSVC>>%SystemRoot%\restart.log
if errorlevel==1 set IISERROR=1

:NOMSGBLDSVC
if %IISERROR%==0 goto EXIT

:ERROR
echo RESTART ERROR...>>%SystemRoot%\restart.log
echo One or more of the services could not be
restarted.>>%SystemRoot%\restart.log
echo Please check the Event Viewer logs for more
information.>>%SystemRoot%\restart.log

REM ********************
REM * Put any desired error-handling commands here.
REM * For example, if you have the Windows NT Resource Kit,
REM * you could use the following command to restart
REM * the server in two minutes:
REM *     SHUTDOWN.EXE /L /R /T:120 /Y
REM ********************

:EXIT
set MSFTPSVC=
set NNTPSVC=
set SMTPSVC=
set W3SVC=
set BROKSVC=
set LDAPSVC=
set MSGBLDSVC=
set IISERROR= 