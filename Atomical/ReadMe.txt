ReadMe.txt
----------

Atomical is a project between Fabio and Robin.

1) Working on Mac, Windows and Linux
2) Build with Qt Creator.  No DevStudio or XCode suport.

Robin's next tasks:
1) Develop the UI - similar to WmCocoa.
2) The ZRotation doesn't rotate the camera on it radial axis.

Fabio's next tasks:
1) Have a break and do his University work.
2) Have a nice time with his colleague from Finland.

Two other matters:

1) now (timestamp)

Fabio:  Here's my script 'now' which will give you the time stamp
#!/bin/bash
python -c "import time; print time.strftime('%Y-%m-%d %H:%M:%S%Z', time.localtime())"

On the Mac, you might like the variant (which puts it on the clipboard)
#!/bin/bash
python -c "import time; print time.strftime('%Y-%m-%d %H:%M:%S%Z', time.localtime())" | pbcopy
pbpaste

2) Linux and Windows

There's something wrong with the OpenGL.
a) Linux = the bitmap is all wrong.
b) Windows = the screen is blank !

This isn't urgent.  It will need attention eventually.

UI
--
Mouse:             rotate the view
MouseWheel:        zoom in/out
MouseWheel+Shift:  Rotate the Q in the view
To quit:           Q or q or Escape

Known Issue
-----------
1) Graphics in Linux and Windows are not right.


Robin Mills
rmills@clanmills.com
http://clanmills.com


Revision History
----------------
2010-11-04 19:16:18PDT  renamed the initX code to be platform
2010-11-05 00:43:00ITT  Implemented fog, methods GLThread::fog_on(float density) and
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
