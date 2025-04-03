@echo off

ECHO =========== Compiling ELENA files ==================

md %~dp0\x64
md %~dp0\x64\bin
md %~dp0\x64\bin\templates
md %~dp0\x64\bin\scripts
md %~dp0\x64\bin\amd64
md %~dp0\x64\doc
md %~dp0\x64\examples60 

copy %~dp0\..\bin\asm64-cli.exe %~dp0\x64\bin
copy %~dp0\..\bin\elena64-cli.exe %~dp0\x64\bin
copy %~dp0\..\bin\elena64-ide.exe %~dp0\x64\bin
copy %~dp0\..\bin\sg64-cli.exe %~dp0\x64\bin
copy %~dp0\..\bin\og64-cli.exe %~dp0\x64\bin
copy %~dp0\..\bin\ecv64-cli.exe %~dp0\x64\bin
copy %~dp0\..\bin\elenart60_64.dll %~dp0\x64\bin
copy %~dp0\..\bin\winstub.ex_ %~dp0\x64\bin
copy %~dp0\..\bin\elc60.cfg %~dp0\x64\bin
copy %~dp0\..\bin\elenart60.cfg %~dp0\x64\bin
copy %~dp0\..\bin\elenavm60.cfg %~dp0\x64\bin

copy %~dp0\..\bin\amd64\core60.bin %~dp0\x64\bin\amd64
copy %~dp0\..\bin\amd64\core60_win.bin %~dp0\x64\bin\amd64
copy %~dp0\..\bin\amd64\corex60.bin %~dp0\x64\bin\amd64
copy %~dp0\..\bin\amd64\core60_win_client.bin %~dp0\x64\bin\amd64

copy %~dp0\..\bin\templates\*.cfg %~dp0\x64\bin\templates\
copy %~dp0\..\bin\scripts\*.es %~dp0\x64\bin\scripts\

copy %~dp0\..\doc\license %~dp0\x64\doc\
copy %~dp0\..\doc\contributors %~dp0\x64\doc\
copy %~dp0\..\readme.md %~dp0\x64\
copy %~dp0\..\CHANGELOG.md %~dp0\x64\
copy %~dp0\..\VERSION %~dp0\x64\

md %~dp0\x64\src60
xcopy %~dp0\..\src60\*.l %~dp0\x64\src60\ /s
xcopy %~dp0\..\src60\*.prj %~dp0\x64\src60\ /s

copy %~dp0\..\src60\elena_api.prjcol %~dp0\x64\src60\

%~dp0\..\bin\sg64-cli.exe %~dp0\..\dat\sg\syntax60.txt
@echo off 
if %ERRORLEVEL% EQU -2 GOTO CompilerError
@echo on

move %~dp0..\dat\sg\syntax60.dat %~dp0\x64\bin\

%~dp0\..\bin\og64-cli %~dp0\..\dat\og\bc_rules60.txt
%~dp0\..\bin\og64-cli -s %~dp0\..\dat\og\bt_rules60.txt

move %~dp0..\dat\og\bt_rules60.dat %~dp0\x64\bin\
move %~dp0..\dat\og\bc_rules60.dat %~dp0\x64\bin\

md %~dp0\lib60_64

%~dp0\..\bin\asm64-cli -bc64 %~dp0\..\src60\core\system.core_routines.esm %~dp0\x64\lib60_64
@echo off 
if %ERRORLEVEL% EQU -2 GOTO CompilerError
@echo on

%~dp0\..\bin\asm64-cli -bc64 %~dp0\..\src60\core\system.win_core_routines.esm %~dp0\x64\lib60_64
@echo off 
if %ERRORLEVEL% EQU -2 GOTO CompilerError
@echo on

%~dp0\x64\bin\elena64-cli %~dp0x64\src60\elena_api.prjcol
@echo off 
if %ERRORLEVEL% EQU -2 GOTO CompilerError
@echo on

%~dp0\..\bin\asm64-cli -amd64 %~dp0\..\asm\amd64\core60.asm bin\amd64
@echo off 
if %ERRORLEVEL% EQU -2 GOTO CompilerError
@echo on

%~dp0\..\bin\asm64-cli -amd64 %~dp0\..\asm\amd64\core60_win.asm bin\amd64
@echo off 
if %ERRORLEVEL% EQU -2 GOTO CompilerError
@echo on

xcopy %~dp0\..\examples60\*.l %~dp0\x64\examples60\ /s
xcopy %~dp0\..\examples60\*.prj %~dp0\x64\examples60\ /s
xcopy %~dp0\..\examples60\*.txt %~dp0\x64\examples60\ /s
xcopy %~dp0\..\examples60\*.bmp %~dp0\x64\examples60\ /s
xcopy %~dp0\..\examples60\*.es %~dp0\x64\examples60\ /s
xcopy %~dp0\..\examples60\*.js %~dp0\x64\examples60\ /s
xcopy %~dp0\..\examples60\*.ls %~dp0\x64\examples60\ /s
xcopy %~dp0\..\examples60\*.xs %~dp0\x64\examples60\ /s

goto:eof
::ERRORS
::---------------------
:CompilerError
echo The MSBuild returns error %ERRORLEVEL%
goto:eof
