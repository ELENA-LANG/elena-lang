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
"%InstallDir%\MSBuild\Current\Bin\MSBuild.exe" elenasrc3\elenasrc3.sln /p:configuration=release /p:Platform="x86" /m:2
IF NOT %ERRORLEVEL%==0 GOTO CompilerError

REM /m:2 is used to build using parallel compilation
"%InstallDir%\MSBuild\Current\Bin\MSBuild.exe" elenasrc3\elenasrc3.sln /p:configuration=release /p:Platform="x64" /m:2 -restore -p:RestorePackagesConfig=true
IF NOT %ERRORLEVEL%==0 GOTO CompilerError

IF "%1"=="-build" GOTO End

ECHO Generating data files
ECHO ----------------------------------------
CALL build\rebuild_data60_x86.bat 
IF NOT %ERRORLEVEL%==0 GOTO CompilerError

ECHO Unit tests
ECHO ----------------------------------------
bin\elena-tests-x86.exe
IF NOT %ERRORLEVEL%==0 GOTO CompilerError

ECHO Generating data files
ECHO ----------------------------------------
CALL build\rebuild_data60_x64.bat 
IF NOT %ERRORLEVEL%==0 GOTO CompilerError

ECHO Unit tests
ECHO ----------------------------------------
bin\elena-tests-x64.exe
IF NOT %ERRORLEVEL%==0 GOTO CompilerError

ECHO =========== Release Compiled ==================

ECHO ============== x86 build ======================
CALL build\rebuild_lib60_x86.bat 
IF NOT %ERRORLEVEL%==0 GOTO CompilerError

ECHO ============== x64 build ======================
CALL build\rebuild_lib60_x64.bat 

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
:SGDataGenError
echo The syntax data file cannnot be generated
goto:eof
:BTDataGenError
echo The build tape rule data file cannnot be generated
goto:eof
:End
echo Build completed
goto:eof
