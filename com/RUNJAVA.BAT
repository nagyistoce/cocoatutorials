@ECHO OFF
REM Copyright (C) Cay S. Horstmann 1997. All rights reserved.
REM Distributed with TextPad with the permission of Cay S. Horstmann,
REM (www.horstmann.com) joint author of "Core Java" (ISBN 0-13-596891-7).
REM
REM This crazy batch file does the following.
REM 1. It finds if the .java file contains the word Applet. If so, it assumes
REM    it is an applet. So don't confuse it with silly things like displaying
REM    "This isn't an applet."
REM 2. If it is an applet, it makes a quick-and-dirty HTML file. I am giving
REM    it the extension .HTM so that your well-crafted HTML files aren't
REM    overwritten. The weird sequence of quotation marks is necessary because
REM    ECHO can't echo unquoted "<" and ">" characters
REM 3. If it is an applet, appletviewer is started. Otherwise the java
REM    interpreter is started.
REM 4. If it is a console applet (not deriving from Frame), then the batch
REM    file pauses for you to admire the output.

if "%1"=="" goto end
if not exist %1.class javac %1.java

find "Applet" %1.java > NUL:
if ERRORLEVEL 1 goto notapplet

echo "<APPLET CODE="%1.class" WIDTH=400 HEIGHT=300 IGNORE=""></APPLET>" > %1.HTM
REM Mercifully, appletviewer ignores the quotes outside the Applet tag
appletviewer %1.HTM

goto end
:notapplet
find "Frame" %1.java > NUL:
if ERRORLEVEL 1 goto notframe

java %1
goto end

:notframe
java %1
pause

:end
