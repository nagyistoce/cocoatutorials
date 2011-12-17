Build this with:

make -B  = Build unconditionally
     "CPPFLAGS=-arch ppc"  (or i386, or ppc64 or x86_64)
     hello
&&   hello                  to execute it

Example:  make -B "CPPFLAGS=-arch ppc" hello && make

With 2.4.1, I found I needed -sysroot <SDKDIR> to be defined to link i386 on PPC
make -B hello 'CPPFLAGS=-arch i386 -arch ppc -isysroot /Developer/SDKs/MacOSX10.4u.sdk'


Detecting 64 bit builds
-----------------------
#ifdef __LP64__ 


60 /Users/rmills/doug> make -B "CPPFLAGS=-arch i386" hello && hello
g++  -arch i386   hello.cpp   -o hello
hello world ptr size = 4 littleEndian, bigEndian = true,false
1 2 3 4 

62 /Users/rmills/doug> make -B "CPPFLAGS=-arch x86_64" hello && hello
g++  -arch x86_64   hello.cpp   -o hello
csh: hello: Bad CPU type in executable.

63 /Users/rmills/doug> make -B "CPPFLAGS=-arch ppc" hello && hello
g++  -arch ppc   hello.cpp   -o hello
hello world ptr size = 4 littleEndian, bigEndian = false,true
4 3 2 1 

64 /Users/rmills/doug> make -B "CPPFLAGS=-arch ppc64" hello && hello
g++  -arch ppc64   hello.cpp   -o hello
csh: hello: Exec format error. Binary file not executable.

66 /Users/rmills/doug> 

+----------------------------------------------------------------------------+
| 4 /Users/rmills/bin/doug> touch empty.cpp ; gcc -E -dM empty.cpp | cet     |
| 5 /Users/rmills/bin/doug> touch empty.c   ; gcc -E -dM empty.c   | cet     |
| 6 /Users/rmills/bin/doug> touch empty.c   ; gcc -E -dM empty.c   > c.txt   |
| 7 /Users/rmills/bin/doug> touch empty.cpp ; gcc -E -dM empty.cpp > cpp.txt |
| 8 /Users/rmills/bin/doug> dir *.txt                                        |
| -rw-r--r--   1 rmills  rmills  2638 Sep  3 10:26 cpp.txt                   |
| -rw-r--r--   1 rmills  rmills  2494 Sep  3 10:26 c.txt                     |
| -rw-r--r--   1 rmills  rmills  1303 Sep  3 10:26 ReadMe.txt                |
| 9 /Users/rmills/bin/doug> ce cpp.txt                                       |
+----------------------------------------------------------------------------+

To build a debuggable version and debug from XCode
--------------------------------------------------
make -B CPPFLAGS=-g hello

In XCode:

1) Create empty project (hello.xcodeproj)
2) Create an executable (custom executable -> hello
3) Open the debugger (^@Y) and add a breakpoint (\@B) to symbol 'main'
4) Execute in debugger (\@Y) and you should stop in main!

@ = Apple key
\ = Alt/Option
^ = Shift

+ -------------------------------------------------------------------+
| MAKE SURE YOU SET a BP in main or the debugger won't stop anywhere |
| stop anywhere.  I think this is because it doesn't read the        |
| symbol table until it stops in main.                               |
|                                                                    |
| Don't even think about debugging until it stops in main!           |
|                                                                    |
+ -------------------------------------------------------------------+

Maybe you can put something in ~/.dgbinit (although I haven't been able to get that to work!)

220 /Users/rmills> cat .gdbinit
echo hello world from ~/.gdbinit
b main
221 /Users/rmills> 


Let Me Say this AGAIN
---------------------

TAKE CARE
I don't think the breakpoints box is effective on external projects without stopping in 'main'
- Sometimes breakpoints are 'lost' because they are not associated with the project
- In this case hit the (-) and things until you get the check (tick) mark
OR
- Make sure you've to the target AND the executable selected before you hit the Debug button

It seems as though the debugger can get started without 'locking' onto the executable.

This has been very frustrating (and will presumably waste lots more time in future)

Other little tips
- - - - - - - - -

New Target... Special Target/Aggregate
Once you have the target, select it and
New Build Phase -> New Run Script Build Phase
	put the commands to build everything in the script
	
Adding source code
- - - - - - - - - -
You can create groups and add source code.  This is convenient for setting breakpoints.
REMEMBER BPs aren't loaded unless the program halts in main !
(at least that's the only way I can get it to work)

Standard I/O Log
- - - - - - - - -
When you're debugging a command line program, you should use the Debug/Standard I/O Log
for input and output.  I've assigned the keys Shift/Apple-L for this purpose using
the XCode/Preferences/keyboard.  You're mileage may vary.

More tips
- - - - - 

http://www.meandmark.com/xcodetips.html

------------------------------------------------------
APPLE DOCUMENTATION
------------------------------------------------------
http://developer.apple.com/documentation/DeveloperTools/Conceptual/XcodeUserGuide/Contents/Resources/en.lproj/06_02_db_set_up_debug/chapter_42_section_6.html


Debugging Arbitrary Applications
You can debug an application in Xcode, even if you do not have the project used to build that application. To do so:
	1.	In Xcode, create an empty project to manage the executable environment for the application. See ÒUsing the New Project AssistantÓ to learn how to create an empty project.
	2.	Create a custom executable environment for the application you want to debug. See ÒCreating Custom Executable EnvironmentsÓ for details.
If you have the source code to the application, you can make that source code available to the debugger so you can see the source for variables and set breakpoints. See ÒExecutable-Environment Debugging InformationÓ to learn how to add source-code directories to an executable debugging environment. This operation makes your source code available to the debugger, but does not give you any of the source code navigation features of Xcode or make the source code available to you to set breakpoints before the debugging sessions starts. For this type of access, you can add the source code directly to your project, as described in ÒAdding Files, Frameworks, and Folders to a Project.Ó
------------------------------------------------------




To link carbonframework from the command line
---------------------------------------------

make -B hello "LDFLAGS=-framework Carbon"

Linking Spidermonkey
--------------------


cc -o Darwin_DBG.OBJ/js.o -c -Wall -Wno-format -g -DXP_UNIX -DSVR4 -DSYSV -D_BSD_SOURCE -DPOSIX_SOURCE -DDARWIN  -DDEBUG -DDEBUG_rmills -DJS_THREADSAFE -DJS_HAS_FILE_OBJECT -DEDITLINE -I../../dist/Darwin_DBG.OBJ/include -IDarwin_DBG.OBJ  js.c
cc -o Darwin_DBG.OBJ/js -Wall -Wno-format -g -DXP_UNIX -DSVR4 -DSYSV -D_BSD_SOURCE -DPOSIX_SOURCE -DDARWIN  -DDEBUG -DDEBUG_rmills -DJS_THREADSAFE -DJS_HAS_FILE_OBJECT -DEDITLINE -I../../dist/Darwin_DBG.OBJ/include -IDarwin_DBG.OBJ  Darwin_DBG.OBJ/js.o Darwin_DBG.OBJ/libjs.a  -Lfdlibm/Darwin_DBG.OBJ -lfdm -L../../dist/Darwin_DBG.OBJ/lib -lnspr4 editline/Darwin_DBG.OBJ/libedit.a

XCode 3.2.1 building 10.5
-------------------------
/Developer/usr/bin/gcc-4.2
-x objective-c
-arch ppc
-fmessage-length=0
-pipe
-std=c99
-Wno-trigraphs
-fpascal-strings
-Os
-mdynamic-no-pic
-Wreturn-type
-Wunused-variable
-isysroot /Developer/SDKs/MacOSX10.5.sdk
-mtune=G5
-fvisibility=hidden
-mmacosx-version-min=10.5
-gdwarf-2
-iquote /Users/rmills/Projects/Favorites/build/Favorites.build/Release/Favorites.build/Favorites-generated-files.hmap
-I/Users/rmills/Projects/Favorites/build/Favorites.build/Release/Favorites.build/Favorites-own-target-headers.hmap
-I/Users/rmills/Projects/Favorites/build/Favorites.build/Release/Favorites.build/Favorites-all-target-headers.hmap
-iquote /Users/rmills/Projects/Favorites/build/Favorites.build/Release/Favorites.build/Favorites-project-headers.hmap
-F/Users/rmills/Projects/Favorites/build/Release
-I/Users/rmills/Projects/Favorites/build/Release/include
-I/Users/rmills/Projects/Favorites/build/Favorites.build/Release/Favorites.build/DerivedSources/ppc
-I/Users/rmills/Projects/Favorites/build/Favorites.build/Release/Favorites.build/DerivedSources
-include /var/folders/tP/tP3k2vHXHjSNd3pECvRec++++TI/-Caches-/com.apple.Xcode.501/SharedPrecompiledHeaders/Favorites_Prefix-fqwwbzcvydkiughhdghfhwclvtvs/Favorites_Prefix.pch
-c /Users/rmills/Projects/Favorites/Controller.m
-o /Users/rmills/Projects/Favorites/build/Favorites.build/Release/Favorites.build/Objects-normal/ppc/Controller.o

Mac Development SDK Macro
-------------------------

-mmacosx-version-min=10.6
#if __ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__ >= 1060

