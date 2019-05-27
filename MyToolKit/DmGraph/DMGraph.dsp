# Microsoft Developer Studio Project File - Name="DMGraph" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=DMGraph - Win32 Unicode Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "DMGraph.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "DMGraph.mak" CFG="DMGraph - Win32 Unicode Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "DMGraph - Win32 Unicode Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "DMGraph - Win32 Unicode Release MinSize" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "DMGraph - Win32 Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "DebugU"
# PROP BASE Intermediate_Dir "DebugU"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "DebugU"
# PROP Intermediate_Dir "DebugU"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "_UNICODE" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "_UNICODE" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comctl32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# Begin Custom Build - Performing registration
OutDir=.\DebugU
TargetPath=.\DebugU\DMGraph.dll
InputPath=.\DebugU\DMGraph.dll
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if "%OS%"=="" goto NOTNT 
	if not "%OS%"=="Windows_NT" goto NOTNT 
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	goto end 
	:NOTNT 
	echo Warning : Cannot register Unicode DLL on Windows 95 
	:end 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "DMGraph - Win32 Unicode Release MinSize"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ReleaseUMinSize"
# PROP BASE Intermediate_Dir "ReleaseUMinSize"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseUMinSize"
# PROP Intermediate_Dir "ReleaseUMinSize"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "_UNICODE" /D "_ATL_DLL" /D "_ATL_MIN_CRT" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "_UNICODE" /Yu"stdafx.h" /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comctl32.lib /nologo /subsystem:windows /dll /machine:I386
# Begin Custom Build - Performing registration
OutDir=.\ReleaseUMinSize
TargetPath=.\ReleaseUMinSize\DMGraph.dll
InputPath=.\ReleaseUMinSize\DMGraph.dll
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if "%OS%"=="" goto NOTNT 
	if not "%OS%"=="Windows_NT" goto NOTNT 
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	goto end 
	:NOTNT 
	echo Warning : Cannot register Unicode DLL on Windows 95 
	:end 
	
# End Custom Build

!ENDIF 

# Begin Target

# Name "DMGraph - Win32 Unicode Debug"
# Name "DMGraph - Win32 Unicode Release MinSize"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AnnoPropPage.cpp
# End Source File
# Begin Source File

SOURCE=.\ColPickerBtn.cpp
# End Source File
# Begin Source File

SOURCE=.\CursorPropPage.cpp
# End Source File
# Begin Source File

SOURCE=.\DMGraph.cpp
# End Source File
# Begin Source File

SOURCE=.\DMGraph.def
# End Source File
# Begin Source File

SOURCE=.\DMGraph.idl
# ADD MTL /tlb ".\DMGraph.tlb" /h "DMGraph.h" /iid "DMGraph_i.c" /Oicf
# End Source File
# Begin Source File

SOURCE=.\DMGraph.rc
# End Source File
# Begin Source File

SOURCE=.\DMGraphCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\DMGraphPropPage.cpp
# End Source File
# Begin Source File

SOURCE=.\DrawFct.cpp
# End Source File
# Begin Source File

SOURCE=.\ElementPropPage.cpp
# End Source File
# Begin Source File

SOURCE=.\FormatPropPage.cpp
# End Source File
# Begin Source File

SOURCE=.\GraphAnno.cpp
# End Source File
# Begin Source File

SOURCE=.\GraphAxis.cpp
# End Source File
# Begin Source File

SOURCE=.\GraphCollection.cpp
# End Source File
# Begin Source File

SOURCE=.\GraphCursor.cpp
# End Source File
# Begin Source File

SOURCE=.\GraphElement.cpp
# End Source File
# Begin Source File

SOURCE=.\PropPageBaseWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\RectTracker.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\about.h
# End Source File
# Begin Source File

SOURCE=.\AnnoPropPage.h
# End Source File
# Begin Source File

SOURCE=.\ColPickerBtn.h
# End Source File
# Begin Source File

SOURCE=.\CursorPropPage.h
# End Source File
# Begin Source File

SOURCE=.\DMGraphCP.h
# End Source File
# Begin Source File

SOURCE=.\DMGraphCtrl.h
# End Source File
# Begin Source File

SOURCE=.\DMGraphPropPage.h
# End Source File
# Begin Source File

SOURCE=.\DMGraphVer.h
# End Source File
# Begin Source File

SOURCE=.\DrawFct.h
# End Source File
# Begin Source File

SOURCE=.\ElementPoint.h
# End Source File
# Begin Source File

SOURCE=.\ElementPropPage.h
# End Source File
# Begin Source File

SOURCE=.\FormatPropPage.h
# End Source File
# Begin Source File

SOURCE=.\GraphAnno.h
# End Source File
# Begin Source File

SOURCE=.\GraphAxis.h
# End Source File
# Begin Source File

SOURCE=.\GraphCollection.h
# End Source File
# Begin Source File

SOURCE=.\GraphCursor.h
# End Source File
# Begin Source File

SOURCE=.\GraphElement.h
# End Source File
# Begin Source File

SOURCE=.\GraphItem.h
# End Source File
# Begin Source File

SOURCE=.\PropPageBaseWnd.h
# End Source File
# Begin Source File

SOURCE=.\RectTracker.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\AnnoPropPage.rgs
# End Source File
# Begin Source File

SOURCE=.\CursorPropPage.rgs
# End Source File
# Begin Source File

SOURCE=.\DMGraph.rc2
# End Source File
# Begin Source File

SOURCE=.\DMGraphc.bmp
# End Source File
# Begin Source File

SOURCE=.\DMGraphCtrl.rgs
# End Source File
# Begin Source File

SOURCE=.\DMGraphPropPage.rgs
# End Source File
# Begin Source File

SOURCE=.\ElementPropPage.rgs
# End Source File
# Begin Source File

SOURCE=.\FormatPropPage.rgs
# End Source File
# Begin Source File

SOURCE=.\Frame.bmp
# End Source File
# Begin Source File

SOURCE=.\GraphCtrl.ico
# End Source File
# End Group
# Begin Source File

SOURCE=.\About.txt
# End Source File
# End Target
# End Project
# Section DMGraph : {00000000-0000-0000-0000-000000000000}
# 	1:24:IDS_TITLEDMGraphPropPage:123
# 	1:19:IDR_DMGraphPROPPAGE:126
# 	1:27:IDS_HELPFILEDMGraphPropPage:124
# 	1:28:IDS_DOCSTRINGDMGraphPropPage:125
# 	1:19:IDD_DMGraphPROPPAGE:127
# End Section
