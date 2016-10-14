# Microsoft Developer Studio Generated NMAKE File, Based on Dedicated.dsp
!IF "$(CFG)" == ""
CFG=Dedicated - Win32 Release
!MESSAGE No configuration specified. Defaulting to Dedicated - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "Dedicated - Win32 Release" && "$(CFG)" != "Dedicated - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Dedicated.mak" CFG="Dedicated - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Dedicated - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Dedicated - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "Dedicated - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\hlds.exe"


CLEAN :
	-@erase "$(INTDIR)\conproc.obj"
	-@erase "$(INTDIR)\dedicated.res"
	-@erase "$(INTDIR)\engine.obj"
	-@erase "$(INTDIR)\sys_ded.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\hlds.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MT /W3 /GX /O2 /I "..\engine" /I "..\common" /I "..\dedicated" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "DEDICATED" /D "LAUNCHERONLY" /Fp"$(INTDIR)\Dedicated.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\dedicated.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Dedicated.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=wsock32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib ..\utils\procinfo\lib\win32_vc6\procinfo.lib /nologo /subsystem:windows /incremental:no /pdb:"$(OUTDIR)\hlds.pdb" /machine:I386 /out:"$(OUTDIR)\hlds.exe" 
LINK32_OBJS= \
	"$(INTDIR)\conproc.obj" \
	"$(INTDIR)\engine.obj" \
	"$(INTDIR)\sys_ded.obj" \
	"$(INTDIR)\dedicated.res"

"$(OUTDIR)\hlds.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Dedicated - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\hlds.exe"


CLEAN :
	-@erase "$(INTDIR)\conproc.obj"
	-@erase "$(INTDIR)\dedicated.res"
	-@erase "$(INTDIR)\engine.obj"
	-@erase "$(INTDIR)\sys_ded.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\hlds.exe"
	-@erase "$(OUTDIR)\hlds.ilk"
	-@erase "$(OUTDIR)\hlds.map"
	-@erase "$(OUTDIR)\hlds.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\engine" /I "..\common" /I "..\dedicated" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "DEDICATED" /D "LAUNCHERONLY" /Fp"$(INTDIR)\Dedicated.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\dedicated.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Dedicated.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=wsock32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib ..\utils\procinfo\lib\win32_vc6\procinfo.lib /nologo /subsystem:windows /incremental:yes /pdb:"$(OUTDIR)\hlds.pdb" /map:"$(INTDIR)\hlds.map" /debug /machine:I386 /out:"$(OUTDIR)\hlds.exe" 
LINK32_OBJS= \
	"$(INTDIR)\conproc.obj" \
	"$(INTDIR)\engine.obj" \
	"$(INTDIR)\sys_ded.obj" \
	"$(INTDIR)\dedicated.res"

"$(OUTDIR)\hlds.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("Dedicated.dep")
!INCLUDE "Dedicated.dep"
!ELSE 
!MESSAGE Warning: cannot find "Dedicated.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "Dedicated - Win32 Release" || "$(CFG)" == "Dedicated - Win32 Debug"
SOURCE=.\conproc.cpp

"$(INTDIR)\conproc.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\dedicated.rc

"$(INTDIR)\dedicated.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\engine.cpp

"$(INTDIR)\engine.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\sys_ded.cpp

"$(INTDIR)\sys_ded.obj" : $(SOURCE) "$(INTDIR)"



!ENDIF 

