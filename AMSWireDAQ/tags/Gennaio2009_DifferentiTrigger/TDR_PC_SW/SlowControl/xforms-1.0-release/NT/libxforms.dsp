# Microsoft Developer Studio Project File - Name="libxforms" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=libxforms - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "libxforms.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "libxforms.mak" CFG="libxforms - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "libxforms - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "libxforms - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "libxforms - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LIBXFORMS_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "d:\program files\devstudio\vc\include" /I "..\..\jpeg-6b" /I "..\FORMS" /I "..\FORMS\private" /I "..\FORMS\fd" /I "..\FORMS\ulib" /I "..\FORMS\bitmaps" /I "..\xpm-3.4k" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LIBXFORMS_EXPORTS" /D "MAKING_FORMS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386

!ELSEIF  "$(CFG)" == "libxforms - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LIBXFORMS_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "d:\program files\devstudio\vc\include" /I "..\..\jpeg-6b" /I "..\FORMS" /I "..\FORMS\private" /I "..\FORMS\fd" /I "..\FORMS\ulib" /I "..\FORMS\bitmaps" /I "..\xpm-3.4k" /D "MAKING_FORMS" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LIBXFORMS_EXPORTS" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept /libpath:"c:\program files\exceed.nt"

!ENDIF 

# Begin Target

# Name "libxforms - Win32 Release"
# Name "libxforms - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\FORMS\align.c
# End Source File
# Begin Source File

SOURCE=..\FORMS\appwin.c
# End Source File
# Begin Source File

SOURCE=..\FORMS\asyn_io.c
# End Source File
# Begin Source File

SOURCE=..\FORMS\be.c
# End Source File
# Begin Source File

SOURCE=..\FORMS\bitmap.c
# End Source File
# Begin Source File

SOURCE=..\FORMS\box.c
# End Source File
# Begin Source File

SOURCE=..\FORMS\browser.c
# End Source File
# Begin Source File

SOURCE=..\FORMS\button.c
# End Source File
# Begin Source File

SOURCE=..\FORMS\canvas.c
# End Source File
# Begin Source File

SOURCE=..\FORMS\chart.c
# End Source File
# Begin Source File

SOURCE=..\FORMS\checkbut.c
# End Source File
# Begin Source File

SOURCE=..\FORMS\child.c
# End Source File
# Begin Source File

SOURCE=..\FORMS\choice.c
# End Source File
# Begin Source File

SOURCE=..\FORMS\clipboard.c
# End Source File
# Begin Source File

SOURCE=..\FORMS\clock.c
# End Source File
# Begin Source File

SOURCE=..\FORMS\cmd_br.c
# End Source File
# Begin Source File

SOURCE=..\FORMS\colsel.c
# End Source File
# Begin Source File

SOURCE=..\FORMS\combo.c
# End Source File
# Begin Source File

SOURCE=..\FORMS\counter.c
# End Source File
# Begin Source File

SOURCE=..\FORMS\cursor.c
# End Source File
# Begin Source File

SOURCE=..\FORMS\dial.c
# End Source File
# Begin Source File

SOURCE=..\FORMS\display.c
# End Source File
# Begin Source File

SOURCE=..\FORMS\ulib\errmsg.c
# End Source File
# Begin Source File

SOURCE=..\FORMS\events.c
# End Source File
# Begin Source File

SOURCE=..\FORMS\flcolor.c
# End Source File
# Begin Source File

SOURCE=..\FORMS\fldraw.c
# End Source File
# Begin Source File

SOURCE=..\FORMS\flresource.c
# End Source File
# Begin Source File

SOURCE=..\FORMS\flvisual.c
# End Source File
# Begin Source File

SOURCE=..\FORMS\fonts.c
# End Source File
# Begin Source File

SOURCE=..\FORMS\formbrowser.c
# End Source File
# Begin Source File

SOURCE=..\FORMS\forms.c
# End Source File
# Begin Source File

SOURCE=..\FORMS\frame.c
# End Source File
# Begin Source File

SOURCE=..\FORMS\free.c
# End Source File
# Begin Source File

SOURCE=..\FORMS\fselect.c
# End Source File
# Begin Source File

SOURCE=..\FORMS\goodie_alert.c
# End Source File
# Begin Source File

SOURCE=..\FORMS\goodie_choice.c
# End Source File
# Begin Source File

SOURCE=..\FORMS\goodie_input.c
# End Source File
# Begin Source File

SOURCE=..\FORMS\goodie_msg.c
# End Source File
# Begin Source File

SOURCE=..\FORMS\goodie_salert.c
# End Source File
# Begin Source File

SOURCE=..\FORMS\goodie_sinput.c
# End Source File
# Begin Source File

SOURCE=..\FORMS\goodie_yesno.c
# End Source File
# Begin Source File

SOURCE=..\FORMS\goodies.c
# End Source File
# Begin Source File

SOURCE=..\FORMS\image.c
# End Source File
# Begin Source File

SOURCE=..\FORMS\image_bmp.c
# End Source File
# Begin Source File

SOURCE=..\FORMS\image_convolve.c
# End Source File
# Begin Source File

SOURCE=..\FORMS\image_crop.c
# End Source File
# Begin Source File

SOURCE=..\FORMS\image_disp.c
# End Source File
# Begin Source File

SOURCE=..\FORMS\image_fits.c
# End Source File
# Begin Source File

SOURCE=..\FORMS\image_genesis.c
# End Source File
# Begin Source File

SOURCE=..\FORMS\image_gif.c
# End Source File
# Begin Source File

SOURCE=..\FORMS\image_gzip.c
# End Source File
# Begin Source File

SOURCE=..\FORMS\image_io_filter.c
# End Source File
# Begin Source File

SOURCE=..\FORMS\image_jpeg.c
# End Source File
# Begin Source File

SOURCE=..\FORMS\image_jquant.c
# End Source File
# Begin Source File

SOURCE=..\FORMS\image_marker.c
# End Source File
# Begin Source File

SOURCE=..\FORMS\image_png.c
# End Source File
# Begin Source File

SOURCE=..\FORMS\image_pnm.c
# End Source File
# Begin Source File

SOURCE=..\FORMS\image_postscript.c
# End Source File
# Begin Source File

SOURCE=..\FORMS\image_proc.c
# End Source File
# Begin Source File

SOURCE=..\FORMS\image_replace.c
# End Source File
# Begin Source File

SOURCE=..\FORMS\image_rotate.c
# End Source File
# Begin Source File

SOURCE=..\FORMS\image_scale.c
# End Source File
# Begin Source File

SOURCE=..\FORMS\image_sgi.c
# End Source File
# Begin Source File

SOURCE=..\FORMS\image_text.c
# End Source File
# Begin Source File

SOURCE=..\FORMS\image_tiff.c
# End Source File
# Begin Source File

SOURCE=..\FORMS\image_type.c
# End Source File
# Begin Source File

SOURCE=..\FORMS\image_warp.c
# End Source File
# Begin Source File

SOURCE=..\FORMS\image_xbm.c
# End Source File
# Begin Source File

SOURCE=..\FORMS\image_xpm.c
# End Source File
# Begin Source File

SOURCE=..\FORMS\image_xwd.c
# End Source File
# Begin Source File

SOURCE=..\FORMS\input.c
# End Source File
# Begin Source File

SOURCE=..\FORMS\interpol.c
# End Source File
# Begin Source File

SOURCE=..\FORMS\keyboard.c
# End Source File
# Begin Source File

SOURCE=..\FORMS\labelbut.c
# End Source File
# Begin Source File

SOURCE=..\FORMS\lframe.c
# End Source File
# Begin Source File

SOURCE=..\FORMS\lightbut.c
# End Source File
# Begin Source File

SOURCE=..\FORMS\listdir.c
# End Source File
# Begin Source File

SOURCE=..\FORMS\matrix.c
# End Source File
# Begin Source File

SOURCE=..\FORMS\menu.c
# End Source File
# Begin Source File

SOURCE=..\FORMS\ulib\n2a.c
# End Source File
# Begin Source File

SOURCE=..\FORMS\objects.c
# End Source File
# Begin Source File

SOURCE=..\FORMS\oneliner.c
# End Source File
# Begin Source File

SOURCE=..\FORMS\pixmap.c
# End Source File
# Begin Source File

SOURCE=..\FORMS\positioner.c
# End Source File
# Begin Source File

SOURCE=..\FORMS\postscript.c
# End Source File
# Begin Source File

SOURCE=..\FORMS\ps_core.c
# End Source File
# Begin Source File

SOURCE=..\FORMS\ps_draw.c
# End Source File
# Begin Source File

SOURCE=..\FORMS\ps_text.c
# End Source File
# Begin Source File

SOURCE=..\FORMS\ulib\read2lsbf.c
# End Source File
# Begin Source File

SOURCE=..\FORMS\ulib\read2msbf.c
# End Source File
# Begin Source File

SOURCE=..\FORMS\ulib\read4lsb.c
# End Source File
# Begin Source File

SOURCE=..\FORMS\ulib\read4msb.c
# End Source File
# Begin Source File

SOURCE=..\FORMS\ulib\readint.c
# End Source File
# Begin Source File

SOURCE=..\FORMS\rgb_db.c
# End Source File
# Begin Source File

SOURCE=..\FORMS\round3d.c
# End Source File
# Begin Source File

SOURCE=..\FORMS\roundbut.c
# End Source File
# Begin Source File

SOURCE=..\FORMS\scrollbar.c
# End Source File
# Begin Source File

SOURCE=..\FORMS\scrollbut.c
# End Source File
# Begin Source File

SOURCE=..\FORMS\signal.c
# End Source File
# Begin Source File

SOURCE=..\FORMS\simu.c
# End Source File
# Begin Source File

SOURCE=..\FORMS\sldraw.c
# End Source File
# Begin Source File

SOURCE=..\FORMS\slider.c
# End Source File
# Begin Source File

SOURCE=..\FORMS\ulib\space.c
# End Source File
# Begin Source File

SOURCE=..\FORMS\spline.c
# End Source File
# Begin Source File

SOURCE=..\FORMS\strdup.c
# End Source File
# Begin Source File

SOURCE=..\FORMS\symbols.c
# End Source File
# Begin Source File

SOURCE=..\FORMS\sysdep.c
# End Source File
# Begin Source File

SOURCE=..\FORMS\tabfolder.c
# End Source File
# Begin Source File

SOURCE=..\FORMS\textbox.c
# End Source File
# Begin Source File

SOURCE=..\FORMS\thumbwheel.c
# End Source File
# Begin Source File

SOURCE=..\FORMS\timeout.c
# End Source File
# Begin Source File

SOURCE=..\FORMS\timer.c
# End Source File
# Begin Source File

SOURCE=..\FORMS\tooltip.c
# End Source File
# Begin Source File

SOURCE=..\FORMS\util.c
# End Source File
# Begin Source File

SOURCE=..\FORMS\valuator.c
# End Source File
# Begin Source File

SOURCE=..\FORMS\version.c
# End Source File
# Begin Source File

SOURCE=..\FORMS\ulib\vn_pair.c
# End Source File
# Begin Source File

SOURCE=..\FORMS\ulib\vstrcat.c
# End Source File
# Begin Source File

SOURCE=..\FORMS\win.c
# End Source File
# Begin Source File

SOURCE=..\FORMS\xdraw.c
# End Source File
# Begin Source File

SOURCE=..\FORMS\xpopup.c
# End Source File
# Begin Source File

SOURCE=..\FORMS\xsupport.c
# End Source File
# Begin Source File

SOURCE=..\FORMS\xtext.c
# End Source File
# Begin Source File

SOURCE=..\FORMS\xyplot.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
