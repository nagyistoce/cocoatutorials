ReadMe.txt
----------

Atomical is a project between Fabio, Robin and Andre.

Qt2 Status
----------

1) Working on Mac and Windows
2) Build with Qt Creator : No DevStudio or XCode

Robin's next tasks:
1) Develop the UI (similar to WmCocoa)
2) The ZRotation doesn't rotate the camera

Fabio's next tasks:
1) Learn to submit code into SVN (I use SmartSVN on the Mac)

UI
--
Mouse:             rotate the view
MouseWheel:        zoom in/out
MouseWheel+Shift:  Rotate the Q in the view
To quit:           Q or q or Escape

Known Issue
-----------
1) Doesn't run on Linux.  To be investigated.

Starting /media/E0325F96325F7090/temp/Atomical_Skeleton/Qt2/Qt2...
1.354570e+00 1.100483e+00 20 4 2
Window::init_double_layer() started
IN SETUP
In calcThread::setup(), mode=2
In GLThread::run() - Initialization
In GLThread::run() - Initialization done, start loop
<unknown>: Fatal IO error 11 (Resource temporarily unavailable) on X server :0.0.
/media/E0325F96325F7090/temp/Atomical_Skeleton/Qt2/Qt2 exited with code 1

Fabio's suggestion is:

#ifdef Q_WS_X11
#include <X11/Xlib.h>
#endif

and in the main(), as the *first* instruction:

#ifdef Q_WS_X11
XInitThreads();
#endif



Robin Mills
rmills@clanmills.com
http://clanmills.com


Revision History
----------------
2010-12-04 00:43:00ITT  Implemented fog, methods GLThread::fog_on(float density) and 
		        GLThread::fog_off(). Now there's a bool bPause to control the 
                        rendering. It is rough: I set it true and wait 20 msecs to be 
                        sure the pending rendering loop is completed. This was needed 
                        since some concurrent calls to GL (such as fog) fail (but do 
                        not crash) if they're performed in the middle of drawing. 
                        Also, this seems to have corrected the problems with the 
                        "smart" autozoom. Resize is still a mess. 
                        Is it so on all platforms?
2010-11-04 13:59:55ITT  Implemented cThread::pause() and cThread::resume(). Implemented
                        GLThread->setAutoZoom(). Implemented a "smart" autoZoom which keeps
                        auto-zooming until the molecule radius is not stable within 1%, then
                        releases control to the user. Great idea, Robin! Sometimes it fails...why?
2010-11-03 23:36:55PDT  Great Progress.  Fabio's done a great job.  Runs well.  Code looks good.
2010-10-38 19:00:00ITT  Multithreaded calculation, OpenGL code in glWidget. Runs (M)
2010-10-27 22:02:39PDT  Integrated Fabio's code.  Compiles, links and works (W/M/L)
2010-10-26 22:07:48PDT  Added Fabio's code - Compiles and links (doesn't work yet)
2010-10-25 21:30:28PDT  First appearance - just to see how/if it works!
