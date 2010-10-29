ReadMe.txt
----------

Atomical is a project between Fabio, Robin and Andre.

Qt2 Status
----------

Working on Mac/Linux and Windows (build with Qt Creator : No DevStudio or XCode)

Robin's next tasks:
1) Develop the UI (similar to WmCocoa)
2) Review all the code.
3) Do something about the CPU load
4) The ZRotation doesn't rotate the camera

Fabio's next tasks:
1) Make a list of tasks!
2) Learn to submit code into SVN (I use SmartSVN on the Mac)

UI
--
Mouse:             rotate the view
MouseWheel:        zoom in/out
MouseWheel+Shift:  Rotate the Q in the view
To quit:           Q or q or Escape

Known Issues
------------
1) ranmar is 'clobbering' the variable i when called in Initialize() on Windows
2) CPU load is 100%
3) Get all those globals into OGLView
4) Avoid all those mallocs of arrays of 5000 items



Robin Mills
rmills@clanmills.com
http://clanmills.com


Revision History
----------------

2010-10-38 19:00:00ITT  Multithreaded calculation, OpenGL code in glWidget. Runs (M)
2010-10-27 22:02:39PDT  Integrated Fabio's code.  Compiles, links and works (W/M/L)
2010-10-26 22:07:48PDT  Added Fabio's code - Compiles and links (doesn't work yet)
2010-10-25 21:30:28PDT  First appearance - just to see how/if it works!
