@echo off

ECHO =========== Compiling ELENA files ==================

md %~dp0\output
md %~dp0\output\bin
md %~dp0\output\bin\templates
md %~dp0\output\bin\scripts
md %~dp0\output\doc
md %~dp0\output\examples 
md %~dp0\output\examples\console 
md %~dp0\output\examples\console\helloworld 
md %~dp0\output\examples\console\sum
md %~dp0\output\examples\console\words
md %~dp0\output\examples\console\pi
md %~dp0\output\examples\console\bsort
md %~dp0\output\examples\console\binary
md %~dp0\output\examples\console\replace
md %~dp0\output\examples\console\goods
md %~dp0\output\examples\files
md %~dp0\output\examples\files\textfile

copy %~dp0\..\bin\asm2binx.exe %~dp0\output\bin
copy %~dp0\..\bin\elc.exe %~dp0\output\bin
copy %~dp0\..\bin\elide.exe %~dp0\output\bin
copy %~dp0\..\bin\sg.exe %~dp0\output\bin
copy %~dp0\..\bin\elt.exe %~dp0\output\bin
copy %~dp0\..\bin\ecv.exe %~dp0\output\bin
copy %~dp0\..\bin\og.exe %~dp0\output\bin
copy %~dp0\..\bin\elenavm.dll %~dp0\output\bin
copy %~dp0\..\bin\elenart.dll %~dp0\output\bin
copy %~dp0\..\bin\winstub.ex_ %~dp0\output\bin
copy %~dp0\..\bin\elc.cfg %~dp0\output\bin
copy %~dp0\..\bin\elenavm.cfg %~dp0\output\bin
copy %~dp0\..\bin\elenart.cfg %~dp0\output\bin

copy %~dp0\..\bin\templates\*.cfg %~dp0\output\bin\templates\
copy %~dp0\..\bin\scripts\*.es %~dp0\output\bin\scripts\

copy %~dp0\..\doc\license %~dp0\output\doc\
copy %~dp0\..\doc\contributors %~dp0\output\doc\
copy %~dp0\..\doc\readme.txt %~dp0\output\

md %~dp0\output\src50\system
xcopy %~dp0\..\src50\system\*.l %~dp0\output\src50\system /s
xcopy %~dp0\..\src50\system\*.prj %~dp0\output\src50\system /s

md %~dp0\output\src50\extensions
xcopy %~dp0\..\src50\extensions\*.l %~dp0\output\src50\extensions /s
xcopy %~dp0\..\src50\extensions\*.prj %~dp0\output\src50\extensions /s

%~dp0\..\bin\sg %~dp0\..\dat\sg\syntax.txt
@echo off 
if %ERRORLEVEL% EQU -2 GOTO CompilerError
@echo on

move %~dp0..\dat\sg\syntax.dat %~dp0\output\bin\

%~dp0\..\bin\og %~dp0\..\dat\og\rules.txt
@echo off 
if %ERRORLEVEL% EQU -2 GOTO CompilerError
@echo on

move %~dp0\..\dat\og\rules.dat %~dp0\output\bin\

%~dp0\..\bin\og s %~dp0\..\dat\og\source_rules.txt 
@echo off 
if %ERRORLEVEL% EQU -2 GOTO CompilerError
@echo on

move %~dp0\..\dat\og\source_rules.dat %~dp0\output\bin\

md %~dp0\lib50
%~dp0\..\bin\asm2binx %~dp0\..\src50\core\system.core_routines.esm %~dp0\output\lib50
@echo off 
if %ERRORLEVEL% EQU -2 GOTO CompilerError
@echo on

%~dp0\output\bin\elc %~dp0\output\src50\system\system.prj
@echo off 
if %ERRORLEVEL% EQU -2 GOTO CompilerError
@echo on

%~dp0\output\bin\elc %~dp0\output\src50\extensions\extensions.prj
@echo off 
if %ERRORLEVEL% EQU -2 GOTO CompilerError
@echo on

%~dp0\..\bin\asm2binx %~dp0\..\asm\x32\core.asm %~dp0\output\bin\x32
@echo off 
if %ERRORLEVEL% EQU -2 GOTO CompilerError
@echo on

%~dp0\..\bin\asm2binx %~dp0\..\asm\x32\coreapi.asm %~dp0\output\bin\x32
@echo off 
if %ERRORLEVEL% EQU -2 GOTO CompilerError
@echo on

%~dp0\..\bin\asm2binx %~dp0\..\asm\x32\core_win.asm %~dp0\output\bin\x32
@echo off 
if %ERRORLEVEL% EQU -2 GOTO CompilerError
@echo on

copy %~dp0\..\examples\console\helloworld\*.l %~dp0\output\examples\console\helloworld
copy %~dp0\..\examples\console\helloworld\*.prj %~dp0\output\examples\console\helloworld

copy %~dp0\..\examples\console\sum\*.l %~dp0\output\examples\console\sum
copy %~dp0\..\examples\console\sum\*.prj %~dp0\output\examples\console\sum

copy %~dp0\..\examples\console\words\*.l %~dp0\output\examples\console\words
copy %~dp0\..\examples\console\words\*.prj %~dp0\output\examples\console\words

copy %~dp0\..\examples\console\pi\*.l %~dp0\output\examples\console\pi
copy %~dp0\..\examples\console\pi\*.prj %~dp0\output\examples\console\pi

copy %~dp0\..\examples\console\bsort\*.l %~dp0\output\examples\console\bsort
copy %~dp0\..\examples\console\bsort\*.prj %~dp0\output\examples\console\bsort

copy %~dp0\..\examples\console\binary\*.l %~dp0\output\examples\console\binary
copy %~dp0\..\examples\console\binary\*.prj %~dp0\output\examples\console\binary

copy %~dp0\..\examples\console\replace\*.l %~dp0\output\examples\console\replace
copy %~dp0\..\examples\console\replace\*.prj %~dp0\output\examples\console\replace

copy %~dp0\..\examples\console\replace\*.l %~dp0\output\examples\console\goods
copy %~dp0\..\examples\console\replace\*.prj %~dp0\output\examples\console\goods
copy %~dp0\..\examples\console\replace\*.txt %~dp0\output\examples\console\goods

copy %~dp0\..\examples\files\textfile\*.l %~dp0\output\files\textfile
copy %~dp0\..\examples\files\textfile\*.prj %~dp0\output\files\textfile
copy %~dp0\..\examples\files\textfile\*.txt %~dp0\output\files\textfile

7z a %~dp0\output\%1.zip %~dp0\output\*.* -r

goto:eof
::ERRORS
::---------------------
:CompilerError
echo The MSBuild returns error %ERRORLEVEL%
goto:eof
