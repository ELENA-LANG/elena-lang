@echo off

ECHO =========== Compiling ELENA files ==================

md %~dp0\output
md %~dp0\output\bin
md %~dp0\output\bin\templates
md %~dp0\output\bin\scripts
md %~dp0\output\doc
md %~dp0\output\examples60 
md %~dp0\output\examples60\console 
md %~dp0\output\examples60\console\helloworld

copy %~dp0\..\bin\asm-cli.exe %~dp0\output\bin
copy %~dp0\..\bin\elena-cli.exe %~dp0\output\bin
copy %~dp0\..\bin\sg-cli.exe %~dp0\output\bin
copy %~dp0\..\bin\ecv-cli.exe %~dp0\output\bin
copy %~dp0\..\bin\elenart60.dll %~dp0\output\bin
copy %~dp0\..\bin\winstub.ex_ %~dp0\output\bin
copy %~dp0\..\bin\elc60.cfg %~dp0\output\bin

copy %~dp0\..\bin\templates\*.cfg %~dp0\output\bin\templates\

copy %~dp0\..\doc\license %~dp0\output\doc\
copy %~dp0\..\doc\contributors %~dp0\output\doc\
copy %~dp0\..\readme.md %~dp0\output\

md %~dp0\output\src60\system
xcopy %~dp0\..\src60\system\*.l %~dp0\output\src60\system /s
xcopy %~dp0\..\src60\system\*.prj %~dp0\output\src60\system /s

md %~dp0\output\src60\extensions
xcopy %~dp0\..\src60\extensions\*.l %~dp0\output\src60\extensions /s
xcopy %~dp0\..\src60\extensions\*.prj %~dp0\output\src60\extensions /s

%~dp0\..\bin\sg-cli.exe %~dp0\..\dat\sg\syntax60.txt
@echo off 
if %ERRORLEVEL% EQU -2 GOTO CompilerError
@echo on

move %~dp0..\dat\sg\syntax60.dat %~dp0\output\bin\

md %~dp0\lib60

%~dp0\..\bin\asm-cli -bc32 %~dp0\..\src60\core\system.core_routines.esm %~dp0\output\lib60
@echo off 
if %ERRORLEVEL% EQU -2 GOTO CompilerError
@echo on

%~dp0\output\bin\elena-cli %~dp0\output\src60\system\system.prj
@echo off 
if %ERRORLEVEL% EQU -2 GOTO CompilerError
@echo on

%~dp0\output\bin\elena-cli %~dp0\output\src60\extensions\extensions.prj
@echo off 
if %ERRORLEVEL% EQU -2 GOTO CompilerError
@echo on

%~dp0\..\bin\asm-cli -x86 %~dp0\..\asm\x32\core60.asm bin\x32
@echo off 
if %ERRORLEVEL% EQU -2 GOTO CompilerError
@echo on

%~dp0\..\bin\asm-cli -x86 %~dp0\..\asm\x32\core60_win.asm bin\x32
@echo off 
if %ERRORLEVEL% EQU -2 GOTO CompilerError
@echo on

copy %~dp0\..\examples60\console\helloworld\*.l %~dp0\output\examples60\console\helloworld
copy %~dp0\..\examples60\console\helloworld\*.prj %~dp0\output\examples60\console\helloworld

7z a %~dp0\output\%1.zip %~dp0\output\*.* %~dp0\output\* -r

goto:eof
::ERRORS
::---------------------
:CompilerError
echo The MSBuild returns error %ERRORLEVEL%
goto:eof
