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
1) Crashes on close down in Linux.  To be investigated.

stopRendering()...done!X Error: BadAccess (attempt to access private resource denied) 10
  Extension:    153 (Uknown extension)
  Minor opcode: 26 (Unknown request)
  Resource id:  0x5200005
QGLContext::makeCurrent(): Failed.
770 /home/rmills/Projects/cocoatutorials/Atomical/Qt2 $ 



Robin Mills
rmills@clanmills.com
http://clanmills.com


Revision History
----------------
2010-11-04 07:57:47PDT  Fixed the Linux build.
2010-11-03 23:36:55PDT  Great Progress.  Fabio's done a great job.  Runs well.  Code looks good.
2010-10-38 19:00:00ITT  Multithreaded calculation, OpenGL code in glWidget. Runs (M)
2010-10-27 22:02:39PDT  Integrated Fabio's code.  Compiles, links and works (W/M/L)
2010-10-26 22:07:48PDT  Added Fabio's code - Compiles and links (doesn't work yet)
2010-10-25 21:30:28PDT  First appearance - just to see how/if it works!
