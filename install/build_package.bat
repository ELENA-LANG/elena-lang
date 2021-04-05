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
md %~dp0\output\examples\console\datetime
md %~dp0\output\examples\console\random
md %~dp0\output\examples\console\trans
md %~dp0\output\examples\console\matrix
md %~dp0\output\examples\console\pi2
md %~dp0\output\examples\files
md %~dp0\output\examples\files\textfile
md %~dp0\output\examples\files\textdb
md %~dp0\output\examples\script\calculator
md %~dp0\output\examples\script\interpreter
md %~dp0\output\examples\script\js
md %~dp0\output\examples\gui\agenda
md %~dp0\output\examples\gui\graphs
md %~dp0\output\examples\gui\helloworld
md %~dp0\output\examples\gui\c_a_g
md %~dp0\output\examples\gui\c_a_g\formulas
md %~dp0\output\examples\gui\c_a_g\formulas\Circulo
md %~dp0\output\examples\gui\c_a_g\formulas\Paralelogramos 
md %~dp0\output\examples\gui\c_a_g\formulas\Trapezio 
md %~dp0\output\examples\gui\c_a_g\formulas\Triangulos 
md %~dp0\output\examples\db\sqlite

copy %~dp0\..\bin\asm2binx.exe %~dp0\output\bin
copy %~dp0\..\bin\elc.exe %~dp0\output\bin
copy %~dp0\..\bin\elide.exe %~dp0\output\bin
copy %~dp0\..\bin\sg.exe %~dp0\output\bin
copy %~dp0\..\bin\elt.exe %~dp0\output\bin
copy %~dp0\..\bin\ecv.exe %~dp0\output\bin
copy %~dp0\..\bin\og.exe %~dp0\output\bin
copy %~dp0\..\bin\elenavm.dll %~dp0\output\bin
copy %~dp0\..\bin\elenart.dll %~dp0\output\bin
copy %~dp0\..\bin\elenasm.dll %~dp0\output\bin
copy %~dp0\..\bin\winstub.ex_ %~dp0\output\bin
copy %~dp0\..\bin\elc.cfg %~dp0\output\bin
copy %~dp0\..\bin\elenavm.cfg %~dp0\output\bin
copy %~dp0\..\bin\elenart.cfg %~dp0\output\bin
copy %~dp0\..\bin\elt.es %~dp0\output\bin
copy %~dp0\..\bin\command.es %~dp0\output\bin

copy %~dp0\..\bin\templates\*.cfg %~dp0\output\bin\templates\
copy %~dp0\..\bin\scripts\*.es %~dp0\output\bin\scripts\

copy %~dp0\..\doc\license %~dp0\output\doc\
copy %~dp0\..\doc\contributors %~dp0\output\doc\
copy %~dp0\..\readme.md %~dp0\output\

md %~dp0\output\src50\system
xcopy %~dp0\..\src50\system\*.l %~dp0\output\src50\system /s
xcopy %~dp0\..\src50\system\*.prj %~dp0\output\src50\system /s

md %~dp0\output\src50\extensions
xcopy %~dp0\..\src50\extensions\*.l %~dp0\output\src50\extensions /s
xcopy %~dp0\..\src50\extensions\*.prj %~dp0\output\src50\extensions /s

md %~dp0\output\src50\sqlite
xcopy %~dp0\..\src50\sqlite\*.l %~dp0\output\src50\sqlite /s
xcopy %~dp0\..\src50\sqlite\*.prj %~dp0\output\src50\sqlite /s

md %~dp0\output\src50\cellular
xcopy %~dp0\..\src50\cellular\*.l %~dp0\output\src50\cellular /s
xcopy %~dp0\..\src50\cellular\*.prj %~dp0\output\src50\cellular /s

md %~dp0\output\src50\forms
xcopy %~dp0\..\src50\forms\*.l %~dp0\output\src50\forms /s
xcopy %~dp0\..\src50\forms\*.prj %~dp0\output\src50\forms /s

md %~dp0\output\src50\xforms
xcopy %~dp0\..\src50\xforms\*.l %~dp0\output\src50\xforms /s
xcopy %~dp0\..\src50\xforms\*.prj %~dp0\output\src50\xforms /s

md %~dp0\output\src50\net
xcopy %~dp0\..\src50\net\*.l %~dp0\output\src50\net /s
xcopy %~dp0\..\src50\net\*.prj %~dp0\output\src50\net /s

md %~dp0\output\src50\algorithms
xcopy %~dp0\..\src50\algorithms\*.l %~dp0\output\src50\algorithms /s
xcopy %~dp0\..\src50\algorithms\*.prj %~dp0\output\src50\algorithms /s

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

rem %~dp0\output\bin\elc %~dp0\output\src50\cellular\cellular.prj
rem @echo off 
rem if %ERRORLEVEL% EQU -2 GOTO CompilerError
rem @echo on

rem %~dp0\output\bin\elc %~dp0\output\src50\sqlite\sqlite.prj
rem @echo off 
rem if %ERRORLEVEL% EQU -2 GOTO CompilerError
rem @echo on

rem %~dp0\output\bin\elc %~dp0\output\src50\forms\forms.prj
rem @echo off 
rem if %ERRORLEVEL% EQU -2 GOTO CompilerError
rem @echo on

rem %~dp0\output\bin\elc %~dp0\output\src50\xforms\xforms.prj
rem @echo off 
rem if %ERRORLEVEL% EQU -2 GOTO CompilerError
rem @echo on

rem %~dp0\output\bin\elc %~dp0\output\src50\net\net.prj
rem @echo off 
rem if %ERRORLEVEL% EQU -2 GOTO CompilerError
rem @echo on

rem %~dp0\output\bin\elc %~dp0\output\src50\algorithms\algorithms.prj
rem @echo off 
rem if %ERRORLEVEL% EQU -2 GOTO CompilerError
rem @echo on

%~dp0\..\bin\asm2binx %~dp0\..\asm\x32\core.asm %~dp0\output\bin\x32
@echo off 
if %ERRORLEVEL% EQU -2 GOTO CompilerError
@echo on

%~dp0\..\bin\asm2binx %~dp0\..\asm\x32\corex.asm %~dp0\output\bin\x32
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

copy %~dp0\..\examples\console\goods\*.l %~dp0\output\examples\console\goods
copy %~dp0\..\examples\console\goods\*.prj %~dp0\output\examples\console\goods
copy %~dp0\..\examples\console\goods\*.txt %~dp0\output\examples\console\goods

copy %~dp0\..\examples\console\datetime\*.l %~dp0\output\examples\console\datetime
copy %~dp0\..\examples\console\datetime\*.prj %~dp0\output\examples\console\datetime

copy %~dp0\..\examples\console\random\*.l %~dp0\output\examples\console\random
copy %~dp0\..\examples\console\random\*.prj %~dp0\output\examples\console\random

copy %~dp0\..\examples\console\trans\*.l %~dp0\output\examples\console\trans
copy %~dp0\..\examples\console\trans\*.prj %~dp0\output\examples\console\trans

copy %~dp0\..\examples\console\matrix\*.l %~dp0\output\examples\console\matrix
copy %~dp0\..\examples\console\matrix\*.prj %~dp0\output\examples\console\matrix

copy %~dp0\..\examples\console\pi2\*.l %~dp0\output\examples\console\pi2
copy %~dp0\..\examples\console\pi2\*.prj %~dp0\output\examples\console\pi2

copy %~dp0\..\examples\files\textfile\*.l %~dp0\output\examples\files\textfile
copy %~dp0\..\examples\files\textfile\*.prj %~dp0\output\examples\files\textfile
copy %~dp0\..\examples\files\textfile\*.txt %~dp0\output\examples\files\textfile

copy %~dp0\..\examples\files\textdb\*.l %~dp0\output\examples\files\textdb
copy %~dp0\..\examples\files\textdb\*.prj %~dp0\output\examples\files\textdb
copy %~dp0\..\examples\files\textdb\*.txt %~dp0\output\examples\files\textdb

copy %~dp0\..\examples\script\calculator\*.l %~dp0\output\examples\script\calculator
copy %~dp0\..\examples\script\calculator\*.prj %~dp0\output\examples\script\calculator
copy %~dp0\..\examples\script\calculator\*.es %~dp0\output\examples\script\calculator

copy %~dp0\..\examples\script\js\*.l %~dp0\output\examples\script\js
copy %~dp0\..\examples\script\js\*.prj %~dp0\output\examples\script\js
copy %~dp0\..\examples\script\js\*.es %~dp0\output\examples\script\js
copy %~dp0\..\examples\script\js\*.js %~dp0\output\examples\script\js

copy %~dp0\..\examples\script\interpreter\*.l %~dp0\output\examples\script\interpreter
copy %~dp0\..\examples\script\interpreter\*.prj %~dp0\output\examples\script\interpreter
copy %~dp0\..\examples\script\interpreter\*.es %~dp0\output\examples\script\interpreter
copy %~dp0\..\examples\script\interpreter\*.txt %~dp0\output\examples\script\interpreter

copy %~dp0\..\examples\gui\agenda\*.l %~dp0\output\examples\gui\agenda
copy %~dp0\..\examples\gui\agenda\*.prj %~dp0\output\examples\gui\agenda

copy %~dp0\..\examples\gui\c_a_g\*.l %~dp0\output\examples\gui\c_a_g
copy %~dp0\..\examples\gui\c_a_g\*.prj %~dp0\output\examples\gui\c_a_g

copy %~dp0\..\examples\gui\c_a_g\formulas\Circulo\*.bmp %~dp0\output\examples\gui\c_a_g\formulas\Circulo
copy %~dp0\..\examples\gui\c_a_g\formulas\Paralelogramos\*.bmp %~dp0\output\examples\gui\c_a_g\formulas\Paralelogramos
copy %~dp0\..\examples\gui\c_a_g\formulas\Trapezio\*.bmp %~dp0\output\examples\gui\c_a_g\formulas\Trapezio
copy %~dp0\..\examples\gui\c_a_g\formulas\Triangulos\*.bmp %~dp0\output\examples\gui\c_a_g\formulas\Triangulos

copy %~dp0\..\examples\gui\graphs\*.l %~dp0\output\examples\gui\graphs
copy %~dp0\..\examples\gui\graphs\*.prj %~dp0\output\examples\gui\graphs

copy %~dp0\..\examples\gui\helloworld\*.l %~dp0\output\examples\gui\helloworld
copy %~dp0\..\examples\gui\helloworld\*.prj %~dp0\output\examples\gui\helloworld
copy %~dp0\..\examples\gui\helloworld\*.xs %~dp0\output\examples\gui\helloworld

copy %~dp0\..\examples\db\sqlite\*.l %~dp0\output\examples\db\sqlite
copy %~dp0\..\examples\db\sqlite\*.prj %~dp0\output\examples\db\sqlite

7z a %~dp0\output\%1.zip %~dp0\output\*.* %~dp0\output\* -r

goto:eof
::ERRORS
::---------------------
:CompilerError
echo The MSBuild returns error %ERRORLEVEL%
goto:eof
