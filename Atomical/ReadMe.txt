ReadMe.txt
----------

Atomical is a project between Fabio and Robin.

Robin's next tasks:

- button for starting the normal modes and a spinbox choosing which eigenmode to display
- calcThread->startEigenmodes(int N)
- calcThread->chooseEigenmode(int N)
- calcThread->stopEigenmodes();
- Precision, Radius, Ratio
  MainWindow::double   separation,imbalance,radsp,precision;

- Preferences storage

- Display information in the feedback zone

- Refactor Qt4 "core" code into a DLL (so,framework)
- Create a screensaver version for Windows/Linux 

(Robin's in England and has no access at the moment to a Mac) 


Fabio's next tasks:
- Give Robin a advice to get the UI correct and to call the correct API
- Rename calcThread.{cpp|h} as WignerModel.{cpp|h} (and variable cThread to be modelThread)

Known issues
------------

No crash on Resize with a MBP 13", OsX 10.6.5 with graphics update 1.0 and NVidia 9400M integrated graphics.
Qt4/Mac/Resize window isn't working correctly

UI
--
Mouse:             rotate the view
MouseWheel:        zoom in/out
MouseWheel+Shift:  Rotate the Q in the view
To quit:           Q or q or Escape

Robin Mills
rmills@clanmills.com
http://clanmills.com


Revision History
----------------
2010-11-24 10:07:02GMT  Updated the TODO list.  Add preference panel.
2010-11-18 23:15:18GMT  UI is more or less complete (I think).
2010-11-16 01:58:38GMT  Fog fixed (and fog check box added to UI)
2010-11-15 21:01:46GMT  Added code to make the links in the about box bring up the browser
2010-11-15 13:18:07GMT  Got resize to work
2010-11-15 03:16:10GMT  Messing (unsuccessfully) with resize
2010-11-14 22:34.00GMT  Fixed a bug in the glThread class (no Np initialized
                        when constructed). 
2010-11-14 09:07:35GMT  Some UI added. MaxNp refactored for dynamic array allocation
                        Please use Printf instead of printf (helps windows debugging)
2010-11-10 20:00:00PST  Added project Qt4 (UI is defined in Qt Designer)
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
