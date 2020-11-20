@echo off

set pre=Microsoft.VisualStudio.Product.
set ids=%pre%Community %pre%Professional %pre%Enterprise %pre%BuildTools
for /f "usebackq tokens=1* delims=: " %%i in (`"%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere" -latest -products %ids% -requires Microsoft.Component.MSBuild`) do (
  if /i "%%i"=="installationPath" set InstallDir=%%j
)

IF NOT EXIST %InstallDir%nul goto MissingMSBuildToolsPath
IF NOT EXIST %InstallDir%\MSBuild\Current\Bin\MSBuild.exe goto MissingMSBuildExe

ECHO =========== Starting Release Compile ==================

ECHO Command line Compiler compiling....
ECHO -----------------------------------
REM /m:2 is used to build using parallel compilation
"%InstallDir%\MSBuild\Current\Bin\MSBuild.exe" elenasrc2\elenasrc2.sln /p:configuration=release /m:2
IF NOT %ERRORLEVEL%==0 GOTO CompilerError

REM /m:2 is used to build using parallel compilation
"%InstallDir%\MSBuild\Current\Bin\MSBuild.exe" elenasrc2\elenasrc2_64.sln /p:configuration=release /m:2
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
