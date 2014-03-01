if NOT DEFINED ACTION set ACTION=/build
if NOT DEFINED release set release=false
if NOT DEFINED debug   set debug=false
if NOT DEFINED static  set static=false
if NOT DEFINED dll     set dll=false
if NOT DEFINED x64     set x64=false
if NOT DEFINED Win32   set Win32=false

if NOT DEFINED INCLUDE       set   "INCLUDE=C:\Program Files (x86)\Microsoft Visual Studio 8\VC\ATLMFC\INCLUDE;C:\Program Files (x86)\Microsoft Visual Studio 8\VC\INCLUDE;C:\Program Files (x86)\Microsoft Visual Studio 8\VC\PlatformSDK\include;c:\Program Files (x86)\Microsoft Visual Studio .NET 2003\SDK\v1.1\include\;c:\home\rmills\dev\win32\boost\include\boost-1_42
if NOT DEFINED LIB           set   "LIB=C:\Program Files (x86)\Microsoft Visual Studio 8\VC\ATLMFC\LIB;C:\Program Files (x86)\Microsoft Visual Studio 8\VC\LIB;C:\Program Files (x86)\Microsoft Visual Studio 8\VC\PlatformSDK\lib;C:\Program Files (x86)\Microsoft Visual Studio 8\SDK\v2.0\lib;c:\Program Files (x86)\Microsoft Visual Studio .NET 2003\SDK\v1.1\Lib\"
if NOT DEFINED LIBPATH       set   "LIBPATH=C:\Windows\Microsoft.NET\Framework\v2.0.50727;C:\Program Files (x86)\Microsoft Visual Studio 8\VC\ATLMFC\LIB"
if NOT DEFINED VS80COMNTOOLS set   "VS80COMNTOOLS=C:\Program Files (x86)\Microsoft Visual Studio 8\Common7\Tools\"
rem  set the build environment
call "%VS80COMNTOOLS%\..\..\Vc\bin\vcvars32.bat"
set
echo .

rem --
rem Pull in the support libraries
cd  ..
if NOT EXIST ..\expat-2.1.0 xcopy/yesihq  c:\exiv2libs\expat-2.1.0 ..\expat-2.1.0
if NOT EXIST ..\zlib-1.2.7  xcopy/yesihq  c:\exiv2libs\zlib-1.2.7  ..\zlib-1.2.7

rem --
rem build and test
cd     msvc2005
if %ACTION%==/clean   set tests=false
if %ACTION%==/upgrade set tests=false
for /f "tokens=*" %%a in ('cygpath -au ..') do set FOO=%%a

if %Win32%==true (
  if %debug%==true (
    if %static%==true (
      devenv exiv2.sln %ACTION% "Debug|Win32"     
      if %tests%==true (
        call bash -c 'cd %FOO%;cd test;./testMSVC.sh ../msvc2005/bin/Win32/Debug'
) ) ) )

if %Win32%==true (
  if %release%==true (
    if %static%==true  (
      devenv exiv2.sln %ACTION% "Release|Win32"    
      if %tests%==true   (
        call bash -c 'cd %FOO%;cd test;./testMSVC.sh ../msvc2005/bin/Win32/Release'
) ) ) )

if %Win32%==true (
  if %debug%==true (
    if %dll%==true   (
      devenv exiv2.sln %ACTION% "DebugDLL|Win32"   
      if %tests%==true (
        call bash -c 'cd %FOO%;cd test;./testMSVC.sh ../msvc2005/bin/Win32/DebugDLL'
) ) ) )

if %Win32%==true (
  if %release%==true (
    if %dll%==true     (
      devenv exiv2.sln %ACTION% "ReleaseDLL|Win32" 
      if %tests%==true   (
        call bash -c 'cd %FOO%;cd test;./testMSVC.sh ../msvc2005/bin/Win32/ReleaseDLL'
) ) ) )

if %x64%==true (
  if %debug%==true (
    if %static%==true (
      devenv exiv2.sln %ACTION% "Debug|x64"        
      if %tests%==true (
        call bash -c 'cd %FOO%;cd test;./testMSVC.sh ../msvc2005/bin/x64/Debug'
) ) ) )

if %x64%==true (
  if %release%==true (
    if %static%==true  (
      devenv exiv2.sln %ACTION% "Release|x64"      
      if %tests%==true   (
        call bash -c 'cd %FOO%;cd test;./testMSVC.sh ../msvc2005/bin/x64/Release'
) ) ) )

if %x64%==true (
  if %debug%==true (
    if %dll%==true   (
      devenv exiv2.sln %ACTION% "DebugDLL|x64"     
      if %tests%==true (
        call bash -c 'cd %FOO%;cd test;./testMSVC.sh ../msvc2005/bin/x64/DebugDLL'
) ) ) )

if %x64%==true   (
  if %release%==true (
    if %dll%==true     (
      devenv exiv2.sln %ACTION% "ReleaseDLL|x64"   
      if %tests%==true   (
        call bash -c 'cd %FOO%;cd test;./testMSVC.sh ../msvc2005/bin/x64/ReleaseDLL'
) ) ) )

rem That's all Folks!
rem -----------------
