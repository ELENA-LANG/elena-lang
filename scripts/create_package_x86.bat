@echo off

ECHO =========== Compiling ELENA files ==================

md %~dp0\x86
md %~dp0\x86\bin
md %~dp0\x86\bin\templates
md %~dp0\x86\bin\scripts
md %~dp0\x86\bin\x32
md %~dp0\x86\doc
md %~dp0\x86\examples60 

copy %~dp0\..\bin\asm-cli.exe %~dp0\x86\bin
copy %~dp0\..\bin\elena-cli.exe %~dp0\x86\bin
copy %~dp0\..\bin\elena-ide.exe %~dp0\x86\bin
copy %~dp0\..\bin\sg-cli.exe %~dp0\x86\bin
copy %~dp0\..\bin\og-cli.exe %~dp0\x86\bin
copy %~dp0\..\bin\ecv-cli.exe %~dp0\x86\bin
copy %~dp0\..\bin\elt-cli.exe %~dp0\x86\bin
copy %~dp0\..\bin\elenart60.dll %~dp0\x86\bin
copy %~dp0\..\bin\elenasm60.dll %~dp0\x86\bin
copy %~dp0\..\bin\elenavm60.dll %~dp0\x86\bin
copy %~dp0\..\bin\winstub.ex_ %~dp0\x86\bin
copy %~dp0\..\bin\elc60.cfg %~dp0\x86\bin
copy %~dp0\..\bin\elenart60.cfg %~dp0\x86\bin
copy %~dp0\..\bin\elenavm60.cfg %~dp0\x86\bin
copy %~dp0\..\bin\elt60.es %~dp0\x86\bin
copy %~dp0\..\bin\command60.es %~dp0\x86\bin

copy %~dp0\..\bin\x86\core60.bin %~dp0\x86\bin\x86\
copy %~dp0\..\bin\x86\core60_win.bin %~dp0\x86\bin\x86\
copy %~dp0\..\bin\x86\core60_win_client.bin %~dp0\x86\bin\x86\
copy %~dp0\..\bin\x86\corex60.bin %~dp0\x86\bin\x86\
copy %~dp0\..\bin\x86\corex60_win.bin %~dp0\x86\bin\x86\

copy %~dp0\..\bin\templates\*.cfg %~dp0\x86\bin\templates\
copy %~dp0\..\bin\scripts\*.es %~dp0\x86\bin\scripts\

copy %~dp0\..\doc\license %~dp0\x86\doc\
copy %~dp0\..\doc\contributors %~dp0\x86\doc\
copy %~dp0\..\readme.md %~dp0\x86\
copy %~dp0\..\CHANGELOG.md %~dp0\x86\
copy %~dp0\..\VERSION %~dp0\x86\

md %~dp0\x86\src60
xcopy %~dp0\..\src60\*.l %~dp0\x86\src60\ /s
xcopy %~dp0\..\src60\*.prj %~dp0\x86\src60\ /s

copy %~dp0\..\src60\elena_api.prjcol %~dp0\x86\src60\

%~dp0\..\bin\sg-cli.exe %~dp0\..\dat\sg\syntax60.txt
@echo off 
if %ERRORLEVEL% EQU -2 GOTO CompilerError
@echo on

move %~dp0..\dat\sg\syntax60.dat %~dp0\x86\bin\

%~dp0\..\bin\og-cli %~dp0\..\dat\og\bc_rules60.txt
%~dp0\..\bin\og-cli -s %~dp0\..\dat\og\bt_rules60.txt
%~dp0\..\bin\og-cli -s %~dp0\..\dat\og\bt_xrules60.txt

move %~dp0..\dat\og\bt_rules60.dat %~dp0\x86\bin\
move %~dp0..\dat\og\bt_xrules60.dat %~dp0\x86\bin\
move %~dp0..\dat\og\bc_rules60.dat %~dp0\x86\bin\

md %~dp0\lib60

%~dp0\..\bin\asm-cli -bc32 %~dp0\..\src60\core\system.core_routines.esm %~dp0\x86\lib60
@echo off 
if %ERRORLEVEL% EQU -2 GOTO CompilerError
@echo on

%~dp0\..\bin\asm-cli -bc32 %~dp0\..\src60\core\system.win_core_routines.esm %~dp0\x86\lib60
@echo off 
if %ERRORLEVEL% EQU -2 GOTO CompilerError
@echo on

%~dp0\x86\bin\elena-cli %~dp0x86\src60\elena_api.prjcol
@echo off 
if %ERRORLEVEL% EQU -2 GOTO CompilerError
@echo on

%~dp0\..\bin\asm-cli -x86 %~dp0\..\asm\x32\core60.asm %~dp0\x86\bin\x32
@echo off 
if %ERRORLEVEL% EQU -2 GOTO CompilerError
@echo on

%~dp0\..\bin\asm-cli -x86 %~dp0\..\asm\x32\core60_win.asm %~dp0\x86\bin\x32
@echo off 
if %ERRORLEVEL% EQU -2 GOTO CompilerError
@echo on

%~dp0\..\bin\asm-cli -x86 %~dp0\..\asm\x32\corex60_win.asm %~dp0\x86\bin\x32
@echo off 
if %ERRORLEVEL% EQU -2 GOTO CompilerError
@echo on

%~dp0\..\bin\asm-cli -x86 %~dp0\..\asm\x32\corex60.asm %~dp0\x86\bin\x32
@echo off 
if %ERRORLEVEL% EQU -2 GOTO CompilerError
@echo on

%~dp0\..\bin\asm-cli -x86 %~dp0\..\asm\x32\core60_win_client.asm %~dp0\x86\bin\x32
@echo off 
if %ERRORLEVEL% EQU -2 GOTO CompilerError
@echo on

xcopy %~dp0\..\examples60\*.l %~dp0\x86\examples60\ /s
xcopy %~dp0\..\examples60\*.prj %~dp0\x86\examples60\ /s
xcopy %~dp0\..\examples60\*.txt %~dp0\x86\examples60\ /s
xcopy %~dp0\..\examples60\*.bmp %~dp0\x86\examples60\ /s
xcopy %~dp0\..\examples60\*.es %~dp0\x86\examples60\ /s
xcopy %~dp0\..\examples60\*.js %~dp0\x86\examples60\ /s
xcopy %~dp0\..\examples60\*.ls %~dp0\x86\examples60\ /s
xcopy %~dp0\..\examples60\*.xs %~dp0\x86\examples60\ /s

goto:eof
::ERRORS
::---------------------
:CompilerError
echo The MSBuild returns error %ERRORLEVEL%
goto:eof
