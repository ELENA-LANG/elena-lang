@echo off

set pre=Microsoft.VisualStudio.Product.
set ids=%pre%Community %pre%Professional %pre%Enterprise %pre%BuildTools
for /f "usebackq tokens=1* delims=: " %%i in (`"%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere" -latest -products %ids% -requires Microsoft.Component.MSBuild`) do (
  if /i "%%i"=="installationPath" set InstallDir=%%j
)

IF NOT EXIST %InstallDir%nul goto MissingMSBuildToolsPath
IF NOT EXIST %InstallDir%\MSBuild\15.0\Bin\MSBuild.exe goto MissingMSBuildExe

ECHO =========== Starting Release Compile ==================

ECHO Command line Compiler compiling....
ECHO -----------------------------------
"%InstallDir%\MSBuild\15.0\Bin\MSBuild.exe" elenasrc2\elc\vs\elc11.vcxproj /p:configuration=release
IF NOT %ERRORLEVEL%==0 GOTO CompilerError

ECHO Virtual Machine compiling....
ECHO -----------------------------
"%InstallDir%\MSBuild\15.0\Bin\MSBuild.exe" elenasrc2\elenavm\vs\elenavm11.vcxproj /p:configuration=release
IF NOT %ERRORLEVEL%==0 GOTO CompilerError

ECHO IDE compiling....
ECHO -----------------
"%InstallDir%\MSBuild\15.0\Bin\MSBuild.exe" elenasrc2\ide\vs\elide11.vcxproj /p:configuration=release
IF NOT %ERRORLEVEL%==0 GOTO CompilerError

ECHO Run-Time Engine compiling....
ECHO -----------------------------
"%InstallDir%\MSBuild\15.0\Bin\MSBuild.exe" elenasrc2\elenart\vs\elenart.vcxproj /p:configuration=release
IF NOT %ERRORLEVEL%==0 GOTO CompilerError

ECHO Script Engine compiling....
ECHO ----------------------------
"%InstallDir%\MSBuild\15.0\Bin\MSBuild.exe" elenasrc2\elenasm\vs\elenasm11.vcxproj /p:configuration=release
IF NOT %ERRORLEVEL%==0 GOTO CompilerError

ECHO Simplified Assembler compiling....
ECHO -----------------------------------
"%InstallDir%\MSBuild\15.0\Bin\MSBuild.exe" elenasrc2\tools\asm2bin\vs\asm2binx11.vcxproj /p:configuration=release
IF NOT %ERRORLEVEL%==0 GOTO CompilerError

ECHO ECODES viewer compiling....
ECHO ---------------------------
"%InstallDir%\MSBuild\15.0\Bin\MSBuild.exe" elenasrc2\tools\ecv\vs\ecv11.vcxproj /p:configuration=release
IF NOT %ERRORLEVEL%==0 GOTO CompilerError

ECHO Virtual Machine Terminal compiling....
ECHO --------------------------------------
"%InstallDir%\MSBuild\15.0\Bin\MSBuild.exe" elenasrc2\tools\elt\vs\elt11.vcxproj /p:configuration=release
IF NOT %ERRORLEVEL%==0 GOTO CompilerError

ECHO Optimization Rule Generator compiling....
ECHO ------------------------------------------
"%InstallDir%\MSBuild\15.0\Bin\MSBuild.exe" elenasrc2\tools\og\vs\og11.vcxproj /p:configuration=release
IF NOT %ERRORLEVEL%==0 GOTO CompilerError

ECHO Syntax Parse Table Generator compiling....
ECHO ------------------------------------------
"%InstallDir%\MSBuild\15.0\Bin\MSBuild.exe" elenasrc2\tools\sg\vs\sg10.vcxproj /p:configuration=release
IF NOT %ERRORLEVEL%==0 GOTO CompilerError

ECHO =========== Release Compiled ==================

goto:eof
::ERRORS
::---------------------
:MissingMSBuildRegistry
echo Cannot obtain path to MSBuild tools from registry
goto:eof
:MissingMSBuildToolsPath
echo The MSBuild tools path from the registry '%InstallDir%' does not exist
goto:eof
:MissingMSBuildExe
echo The MSBuild executable could not be found at '%InstallDir%'
goto:eof
:CompilerError
echo The MSBuild returns error %ERRORLEVEL%
goto:eof
