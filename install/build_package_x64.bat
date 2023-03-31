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
md %~dp0\output\examples60\console\sum
md %~dp0\output\examples60\console\words
md %~dp0\output\examples60\rosetta\accumulator
md %~dp0\output\examples60\rosetta\ackermann
md %~dp0\output\examples60\rosetta\addfield
md %~dp0\output\examples60\rosetta\anonymrec
md %~dp0\output\examples60\rosetta\aplusb
md %~dp0\output\examples60\rosetta\applycallback
md %~dp0\output\examples60\rosetta\arithmeticint
md %~dp0\output\examples60\rosetta\arithmeval
md %~dp0\output\examples60\rosetta\arithmmean
md %~dp0\output\examples60\rosetta\arrayconcat
md %~dp0\output\examples60\rosetta\arraymode

copy %~dp0\..\bin\asm64-cli.exe %~dp0\output\bin
copy %~dp0\..\bin\elena64-cli.exe %~dp0\output\bin
copy %~dp0\..\bin\elena64-ide.exe %~dp0\output\bin
copy %~dp0\..\bin\sg64-cli.exe %~dp0\output\bin
copy %~dp0\..\bin\og64-cli.exe %~dp0\output\bin
copy %~dp0\..\bin\ecv64-cli.exe %~dp0\output\bin
copy %~dp0\..\bin\elenart60_64.dll %~dp0\output\bin
copy %~dp0\..\bin\winstub.ex_ %~dp0\output\bin
copy %~dp0\..\bin\elc60.cfg %~dp0\output\bin
copy %~dp0\..\bin\elenart60.cfg %~dp0\output\bin
copy %~dp0\..\bin\elenavm60.cfg %~dp0\output\bin

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

%~dp0\..\bin\sg64-cli.exe %~dp0\..\dat\sg\syntax60.txt
@echo off 
if %ERRORLEVEL% EQU -2 GOTO CompilerError
@echo on

move %~dp0..\dat\sg\syntax60.dat %~dp0\output\bin\

%~dp0\..\bin\og64-cli -s %~dp0\..\dat\og\bt_rules.txt 

md %~dp0\lib60_64

%~dp0\..\bin\asm64-cli -bc64 %~dp0\..\src60\core\system.core_routines.esm %~dp0\output\lib60_64
@echo off 
if %ERRORLEVEL% EQU -2 GOTO CompilerError
@echo on

%~dp0\output\bin\elena64-cli %~dp0\output\src60\system\system.prj
@echo off 
if %ERRORLEVEL% EQU -2 GOTO CompilerError
@echo on

%~dp0\output\bin\elena64-cli %~dp0\output\src60\extensions\extensions.prj
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

copy %~dp0\..\examples60\console\helloworld\*.l %~dp0\output\examples60\console\helloworld
copy %~dp0\..\examples60\console\helloworld\*.prj %~dp0\output\examples60\console\helloworld

copy %~dp0\..\examples60\console\sum\*.l %~dp0\output\examples60\console\sum
copy %~dp0\..\examples60\console\sum\*.prj %~dp0\output\examples60\console\sum

copy %~dp0\..\examples60\console\words\*.l %~dp0\output\examples60\console\words

copy %~dp0\..\examples60\rosetta\accumulator\*.l %~dp0\output\examples60\rosetta\accumulator
copy %~dp0\..\examples60\rosetta\ackermann\*.l %~dp0\output\examples60\rosetta\ackermann
copy %~dp0\..\examples60\rosetta\addfield\*.l %~dp0\output\examples60\rosetta\addfield
copy %~dp0\..\examples60\rosetta\anonymrec\*.l %~dp0\output\examples60\rosetta\anonymrec
copy %~dp0\..\examples60\rosetta\aplusb\*.l %~dp0\output\examples60\rosetta\aplusb
copy %~dp0\..\examples60\rosetta\applycallback\*.l %~dp0\output\examples60\rosetta\applycallback
copy %~dp0\..\examples60\rosetta\arithmeticint\*.l %~dp0\output\examples60\rosetta\arithmeticint
copy %~dp0\..\examples60\rosetta\arithmeval\*.l %~dp0\output\examples60\rosetta\arithmeval
copy %~dp0\..\examples60\rosetta\arithmmean\*.l %~dp0\output\examples60\rosetta\arithmmean
copy %~dp0\..\examples60\rosetta\arrayconcat\*.l %~dp0\output\examples60\rosetta\arrayconcat
copy %~dp0\..\examples60\rosetta\arraymode\*.l %~dp0\output\examples60\rosetta\arraymode

7z a %~dp0\output\%1.zip %~dp0\output\*.* %~dp0\output\* -r

goto:eof
::ERRORS
::---------------------
:CompilerError
echo The MSBuild returns error %ERRORLEVEL%
goto:eof
