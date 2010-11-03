Robin's Screen Saver
--------------------

I build this from the following sources:
http://cocoadevcentral.com/articles/000088.php

The purpose of the project is to build with XCode 3.2.  There are 4 Configurations:

Release32 and Debug32    - these are 32 bit ppc/intel builds (for Tiger and Leopard)
Release64 and Release64  - these are 64 bit intel builds for Snow Leopard.

Here's what I've done:
1) created the project with the XCode Wizard as "RobinsScreenSaver"
2) Copied the code from MyScreenSaverView.{m|h} to RobinsScreenSaverView.{m|h}
3) Copied ConfigureInfo.nib to Configure.nib and modified code appropriately.
4) Build the debug and release builds and tried them on Snow Leopard
5) I set up /Applications/SystemPreferences.app to be the debugee and debugged into the code.

Installing, Running and Debugging
---------------------------------

To install the Screen Saver, I think you can run it from XCode, or

cd RobinsScreenSaver/builds/Release64
open RobinsScreenSaver.saver and it will offer to install into the SystemPreferences.
Close SystemPreferences if you want to debug it.  You can set breakpoints in the
usual way.  When SysPrefs starts (in the debugger), select RobinsScreenSaver and debug.

Creating the build environment for the 32bit versions
-----------------------------------------------------

For the 64/32 builds, I modified the following:

1) Renamed Debug and Release as Debug64 and Release64
2) Duplicated Debug64 and Release64 as Debug32 and Release32
3) Change the settings in Debug32 and Release32 to be:
   a) Architectures = 32-bit Universal
   b) BaseSDK = Mac OS X 10.4
   c) C/C++ = GCC 4.0
4) Build everything and see what we have.


536 $ cd /Users/rmills/Projects/RobinsScreenSaver
537 $ for c in Debug32 Release32 Debug64 Release64 ;do xcodebuild -project RobinsScreenSaver.xcodeproj -configuration $c clean build ; done
538 $ find . -name RobinsScreenSaver -exec lipo -info {} ";"
539 $ find . -name RobinsScreenSaver -exec otool -L {} ";"

Architectures in the fat file: ./build/Debug32/RobinsScreenSaver.saver/Contents/MacOS/RobinsScreenSaver are: i386 ppc 
Non-fat file:                  ./build/Debug64/RobinsScreenSaver.saver/Contents/MacOS/RobinsScreenSaver is architecture: x86_64
Architectures in the fat file: ./build/Release32/RobinsScreenSaver.saver/Contents/MacOS/RobinsScreenSaver are: i386 ppc 
Non-fat file:                  ./build/Release64/RobinsScreenSaver.saver/Contents/MacOS/RobinsScreenSaver is architecture: x86_64

I've tested the 32 bit builds on Tiger/PPC, Leopard/Intel
I've tested the 64 bit builds on SnowLeopard, Intel

Robin Mills
robin@clanmills.com
http://clanmills.com
2010-11-02 19:50:01PDT

-- end --
