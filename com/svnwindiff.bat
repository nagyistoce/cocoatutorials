@echo off
rem attrib -h .svn
rem args %*%
set SVN=%6%
set ME=%7%
rem pause
rem echo         %5%
windiff %SVN% %ME%
rem attrib +h .svn

