@echo off

reg.exe query "HKLM\SOFTWARE\Microsoft\MSBuild\ToolsVersions\14.0" /v MSBuildToolsPath > nul 2>&1
if ERRORLEVEL 1 goto MissingMSBuildRegistry

for /f "skip=2 tokens=2,*" %%A in ('reg.exe query "HKLM\SOFTWARE\Microsoft\MSBuild\ToolsVersions\4.0" /v MSBuildToolsPath') do SET MSBUILDDIR=%%B

IF NOT EXIST %MSBUILDDIR%nul goto MissingMSBuildToolsPath
IF NOT EXIST %MSBUILDDIR%msbuild.exe goto MissingMSBuildExe

ECHO =========== Starting Release Compile ==================

ECHO Command line Compiler compiling....
ECHO -----------------------------------
"%MSBUILDDIR%msbuild.exe" elenasrc2\elc\vs\elc11.vcxproj /p:configuration=release
IF NOT %ERRORLEVEL%==0 GOTO CompilerError

ECHO Virtual Machine compiling....
ECHO -----------------------------
"%MSBUILDDIR%msbuild.exe" elenasrc2\elenavm\vs\elenavm11.vcxproj /p:configuration=release
IF NOT %ERRORLEVEL%==0 GOTO CompilerError

ECHO IDE compiling....
ECHO -----------------
"%MSBUILDDIR%msbuild.exe" elenasrc2\ide\vs\elide11.vcxproj /p:configuration=release
IF NOT %ERRORLEVEL%==0 GOTO CompilerError

ECHO Run-Time Engine compiling....
ECHO -----------------------------
"%MSBUILDDIR%msbuild.exe" elenasrc2\elenart\vs\elenart.vcxproj /p:configuration=release
IF NOT %ERRORLEVEL%==0 GOTO CompilerError

ECHO Script Engine compiling....
ECHO ----------------------------
"%MSBUILDDIR%msbuild.exe" elenasrc2\elenasm\vs\elenasm11.vcxproj /p:configuration=release
IF NOT %ERRORLEVEL%==0 GOTO CompilerError

ECHO Simplified Assembler compiling....
ECHO -----------------------------------
"%MSBUILDDIR%msbuild.exe" elenasrc2\tools\asm2bin\vs\asm2binx11.vcxproj /p:configuration=release
IF NOT %ERRORLEVEL%==0 GOTO CompilerError

ECHO ECODES viewer compiling....
ECHO ---------------------------
"%MSBUILDDIR%msbuild.exe" elenasrc2\tools\ecv\vs\ecv11.vcxproj /p:configuration=release
IF NOT %ERRORLEVEL%==0 GOTO CompilerError

ECHO Virtual Machine Terminal compiling....
ECHO --------------------------------------
"%MSBUILDDIR%msbuild.exe" elenasrc2\tools\elt\vs\elt11.vcxproj /p:configuration=release
IF NOT %ERRORLEVEL%==0 GOTO CompilerError

ECHO Optimization Rule Generator compiling....
ECHO ------------------------------------------
"%MSBUILDDIR%msbuild.exe" elenasrc2\tools\og\vs\og11.vcxproj /p:configuration=release
IF NOT %ERRORLEVEL%==0 GOTO CompilerError

ECHO Syntax Parse Table Generator compiling....
ECHO ------------------------------------------
"%MSBUILDDIR%msbuild.exe" elenasrc2\tools\sg\vs\sg10.vcxproj /p:configuration=release
IF NOT %ERRORLEVEL%==0 GOTO CompilerError

ECHO =========== Release Compiled ==================

goto:eof
::ERRORS
::---------------------
:MissingMSBuildRegistry
echo Cannot obtain path to MSBuild tools from registry
goto:eof
:MissingMSBuildToolsPath
echo The MSBuild tools path from the registry '%MSBUILDDIR%' does not exist
goto:eof
:MissingMSBuildExe
echo The MSBuild executable could not be found at '%MSBUILDDIR%'
goto:eof
:CompilerError
echo The MSBuild returns error %ERRORLEVEL%
goto:eof
