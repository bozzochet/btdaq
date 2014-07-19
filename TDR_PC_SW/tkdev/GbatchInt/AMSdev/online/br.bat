@rem --- ROOT BUILD SCRIPT ---
@rem Customize CYGWINROOT to point to your cygwin installation's root directory
@set CYGWINROOT=C:\cygwin
@%CYGWINROOT%\bin\bash --login -c ' export AMSWD=c:/cygwin/home/Administrator/gdk/root; export ROOTSYS=c:/cygwin/home/Administrator/gdk/root  ; cd  /choutko/AMS/online; make %1'

