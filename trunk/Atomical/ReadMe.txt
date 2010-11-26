ReadMe.txt
----------

Atomical is a project between Fabio and Robin.

Robin's next tasks:
- Fabio's list (see details below)
  1 Default displayMenubar = false (done)
  2 Fog to respect background color (done)
    Help (Fabio volunteered and can update MainWindow::sHelpURL when ready)
  3 Spin button for Np
  4 Spin button for Np2 (int range 0..Np)
  5 No action required (comment about something)
  6 Coordinate enable/disable Modes/EigenvalueMax (done, to be tested)
  7 Change prec to imbalance (and wire to model, range double 0.0 .. 10.0)
  8 Add a "Surprise" button (random problem).  New problem should be less violent!
  9 Wire rad to radsp in the model (range double 0.0 .. 3.0)
 10 Feedback Display: energy, radius of the molecule, convergence, and the number of particles within the sphere
- Get Preferences to persist on all platforms (done, to be tested)
- Refactor Qt4 "core" code into a DLL (so,framework)
- Create a screensaver version for Windows/Linux 

Fabio's next tasks:
- Rename calcThread.{cpp|h} as WignerModel.{cpp|h} (and variable cThread to be modelThread)

Known issues
------------

+----------------------------------------------+
| This isn't working very well on Windows      |
| Robin's going to do some digging about       |
| - vibration modes are slow (timing, I think) |
| - the eigenModeMax stuff is suspect          |
+----------------------------------------------+

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


----
-- email from Fabio --
1) I would choose as a default to have the menubar disabled in fullscreen mode.

Choosing backgroud is also very nice.

2) We need to "propagate" the RGB components of the bgnd to the GLThread code where fog is, so that we change its colour to the background one (otherwise, fog looks funny). I also *love* the colourwell option!!!

Preferences are also a very nice addition.

The web help page...I like it! I will take care of writing documentation if you like!

Concerning controls, we are 90% there. I'll tell you how the UI should work in my opinion

3) The number of particles should be set by a spinbox, similar to the one for the Emodes. A slider for the range 3...5000 is just not very precise. Alternatively, we can leave the slider but make it "interactive" so that the label changes *while* one drags the slider (in Cocoa, this is called "continuous" mode).
4) Also for Np2, the number of particles in layer 2, there should be a slider/spinbox.
5) I like the idea of having normal modes calculation set as a check box.
6) The spinbox for Emodes should have a maximum value set at the maximum eigenmode allowed compatible with Np. When you change Np, that maximum value should be changed. If one chooses Np>100, for which eigenmodes are not calculated anymore, the spinbox and the check box should be grayed out/disabled. The box should also be gray until the modes are actually calculated. The, changing the number should change the displayed mode (chooseEigenmode(n) method of calcthread). If one restarts with a new problem, the box is gray until eigenmodes are calculated & so on...
7) I would swap the "prec" slider with one setting "imbalance". The behaviour of "imbalance" and "separation" should be that of *changing* the value but not reinitializing the problem. This way, you could start with a cluster of Np=10 particles and Np=3, separation=0 and then, by increasing it, see three particles "separating" from the main cluster. This can be done by calling initProblem() with FLAG (the last
parameter) set to zero.
8) In general, restarting a new problem should not reshuffle parameters, all should be set by the user. If we want, we can add a "Surprise!" button which chooses random parameters and sets the UI accordingly.
9) The radius slider should set radsp. This is used in the GLThread to render a yellow translucent sphere used to count the particles. In turns, this number of particles should be displayed into the UI...
10) ...into the nice "feedback" space you left! There, I'd put the energy, radius of the molecule, convergence and the number of particles within the sphere (got from within the GLThread).

Here are parameter ranges which work:

*) Separation: from 0 to 3. Double.
*) Imbalance: from 1 to 10. Double.
*) Np2: from 0 to Np.
-- end of email --
----

Revision History
----------------
2010-11-26 10:40:25GMT  Updated this file. 14:15 Update:  Preferences persist and keystrokes work on Windows.
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
