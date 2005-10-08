# Microsoft Developer Studio Project File - Name="ChewingIME" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=ChewingIME - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ChewingIME.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ChewingIME.mak" CFG="ChewingIME - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ChewingIME - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ChewingIME - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ChewingIME - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ChewingIME___Win32_Release"
# PROP BASE Intermediate_Dir "ChewingIME___Win32_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "CHEWINGIME_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "..\..\libchewing\branches\win32\include" /D "NDEBUG" /D "NOIME" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "CHEWINGIME_EXPORTS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x404 /d "NDEBUG"
# ADD RSC /l 0x404 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib imm32.lib libchewing.lib comctl32.lib /nologo /dll /machine:I386 /out:"Release/Chewing.ime" /libpath:"..\..\libchewing\branches\win32\win32\Release"

!ELSEIF  "$(CFG)" == "ChewingIME - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "ChewingIME___Win32_Debug"
# PROP BASE Intermediate_Dir "ChewingIME___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "CHEWINGIME_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\..\libchewing\branches\win32\include" /D "_DEBUG" /D "NOIME" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "CHEWINGIME_EXPORTS" /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x404 /d "_DEBUG"
# ADD RSC /l 0x404 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib imm32.lib libchewing.lib comctl32.lib /nologo /dll /debug /machine:I386 /out:"Debug/Chewing.ime" /pdbtype:sept /libpath:"..\..\libchewing\branches\win32\win32\Release"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=@echo on	del C:\Windows\System32\Chewing.ime	copy .\Debug\Chewing.ime c:\windows\system32\Chewing.ime
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "ChewingIME - Win32 Release"
# Name "ChewingIME - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\CandList.cpp
# End Source File
# Begin Source File

SOURCE=.\CandWnd.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libchewing\branches\win32\src\char.c
# End Source File
# Begin Source File

SOURCE=.\Chewing.cpp
# End Source File
# Begin Source File

SOURCE=.\ChewingIME.cpp
# End Source File
# Begin Source File

SOURCE=.\ChewingIME.def
# End Source File
# Begin Source File

SOURCE=.\ChewingIME.rc
# End Source File
# Begin Source File

SOURCE=..\..\libchewing\branches\win32\src\chewingio.c
# End Source File
# Begin Source File

SOURCE=..\..\libchewing\branches\win32\src\chewingutil.c
# End Source File
# Begin Source File

SOURCE=..\..\libchewing\branches\win32\src\choice.c
# End Source File
# Begin Source File

SOURCE=.\CompStr.cpp
# End Source File
# Begin Source File

SOURCE=.\CompWnd.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libchewing\branches\win32\src\dict.c
# End Source File
# Begin Source File

SOURCE=.\DrawUtil.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libchewing\branches\win32\src\hanyupinying.c
# End Source File
# Begin Source File

SOURCE=..\..\libchewing\branches\win32\src\hash.c
# End Source File
# Begin Source File

SOURCE=.\IMCLock.cpp
# End Source File
# Begin Source File

SOURCE=.\IMEChildWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\IMEUI.cpp
# End Source File
# Begin Source File

SOURCE=.\IMEUILock.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libchewing\branches\win32\src\common\key2pho.c
# End Source File
# Begin Source File

SOURCE=.\StatusWnd.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libchewing\branches\win32\src\tree.c
# End Source File
# Begin Source File

SOURCE=..\..\libchewing\branches\win32\src\userphrase.c
# End Source File
# Begin Source File

SOURCE=.\Window.cpp
# End Source File
# Begin Source File

SOURCE=.\XPToolbar.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libchewing\branches\win32\src\zuin.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\CandList.h
# End Source File
# Begin Source File

SOURCE=.\CandWnd.h
# End Source File
# Begin Source File

SOURCE=.\ChewingIME.h
# End Source File
# Begin Source File

SOURCE=.\Chewingpp.h
# End Source File
# Begin Source File

SOURCE=.\CompStr.h
# End Source File
# Begin Source File

SOURCE=.\CompWnd.h
# End Source File
# Begin Source File

SOURCE=.\DrawUtil.h
# End Source File
# Begin Source File

SOURCE=.\IMCLock.h
# End Source File
# Begin Source File

SOURCE=.\IMEChildWnd.h
# End Source File
# Begin Source File

SOURCE=.\IMEUI.h
# End Source File
# Begin Source File

SOURCE=.\IMEUILock.h
# End Source File
# Begin Source File

SOURCE=.\imm.h
# End Source File
# Begin Source File

SOURCE=..\..\libchewing\branches\win32\src\private.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\StatusWnd.h
# End Source File
# Begin Source File

SOURCE=.\Window.h
# End Source File
# Begin Source File

SOURCE=.\XPToolbar.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\chi.ico
# End Source File
# Begin Source File

SOURCE=.\config.ico
# End Source File
# Begin Source File

SOURCE=.\config1.ico
# End Source File
# Begin Source File

SOURCE=.\congi.ico
# End Source File
# Begin Source File

SOURCE=.\eng.ico
# End Source File
# Begin Source File

SOURCE=.\full.ico
# End Source File
# Begin Source File

SOURCE=.\fullshap.ico
# End Source File
# Begin Source File

SOURCE=.\half.ico
# End Source File
# Begin Source File

SOURCE=.\icon.bmp
# End Source File
# Begin Source File

SOURCE=.\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\icon2.ico
# End Source File
# Begin Source File

SOURCE=.\icon3.ico
# End Source File
# Begin Source File

SOURCE=.\status_bar24.bmp
# End Source File
# Begin Source File

SOURCE=.\status_bar256.bmp
# End Source File
# End Group
# End Target
# End Project
