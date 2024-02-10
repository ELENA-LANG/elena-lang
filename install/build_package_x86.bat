@echo off

ECHO =========== Compiling ELENA files ==================

md %~dp0\output32
md %~dp0\output32\bin
md %~dp0\output32\bin\templates
md %~dp0\output32\bin\scripts
md %~dp0\output32\bin\x32
md %~dp0\output32\doc
md %~dp0\output32\examples60 
md %~dp0\output32\examples60\console 
md %~dp0\output32\examples60\console\helloworld
md %~dp0\output32\examples60\console\sum
md %~dp0\output32\examples60\console\words
md %~dp0\output32\examples60\console\binary 
md %~dp0\output32\examples60\console\bsort 
md %~dp0\output32\examples60\console\datetime 
md %~dp0\output32\examples60\console\goods 
md %~dp0\output32\examples60\console\replace 
md %~dp0\output32\examples60\console\pi
md %~dp0\output32\examples60\scripts\calc
md %~dp0\output32\examples60\rosetta\accumulator
md %~dp0\output32\examples60\rosetta\ackermann
md %~dp0\output32\examples60\rosetta\addfield
md %~dp0\output32\examples60\rosetta\anonymrec
md %~dp0\output32\examples60\rosetta\aplusb
md %~dp0\output32\examples60\rosetta\applycallback
md %~dp0\output32\examples60\rosetta\arithmeticint
md %~dp0\output32\examples60\rosetta\arithmeval
md %~dp0\output32\examples60\rosetta\arithmmean
md %~dp0\output32\examples60\rosetta\arrayconcat
md %~dp0\output32\examples60\rosetta\arraymode
md %~dp0\output32\examples60\rosetta\arrays 
md %~dp0\output32\examples60\rosetta\associativearrays 
md %~dp0\output32\examples60\rosetta\bestshuffle 
md %~dp0\output32\examples60\rosetta\binary 
md %~dp0\output32\examples60\rosetta\bitwise 
md %~dp0\output32\examples60\rosetta\brackets 
md %~dp0\output32\examples60\rosetta\bullscows 
md %~dp0\output32\examples60\rosetta\caesar 
md %~dp0\output32\examples60\rosetta\calendar 
md %~dp0\output32\examples60\rosetta\charmatch 
md %~dp0\output32\examples60\rosetta\combinations 
md %~dp0\output32\examples60\rosetta\doors 
md %~dp0\output32\examples60\rosetta\evolutionary 
md %~dp0\output32\examples60\rosetta\firstclass 
md %~dp0\output32\examples60\rosetta\loop_multiple_arrays 
md %~dp0\output32\examples60\rosetta\median 
md %~dp0\output32\examples60\rosetta\ninetynine 
md %~dp0\output32\examples60\rosetta\reverse_words_in_string 
md %~dp0\output32\examples60\rosetta\smavg 
md %~dp0\output32\examples60\rosetta\string_append 

copy %~dp0\..\bin\asm-cli.exe %~dp0\output32\bin
copy %~dp0\..\bin\elena-cli.exe %~dp0\output32\bin
copy %~dp0\..\bin\elena-ide.exe %~dp0\output32\bin
copy %~dp0\..\bin\sg-cli.exe %~dp0\output32\bin
copy %~dp0\..\bin\og-cli.exe %~dp0\output32\bin
copy %~dp0\..\bin\ecv-cli.exe %~dp0\output32\bin
copy %~dp0\..\bin\elt-cli.exe %~dp0\output32\bin
copy %~dp0\..\bin\elenart60.dll %~dp0\output32\bin
copy %~dp0\..\bin\elenavm60.dll %~dp0\output32\bin
copy %~dp0\..\bin\winstub.ex_ %~dp0\output32\bin
copy %~dp0\..\bin\elc60.cfg %~dp0\output32\bin
copy %~dp0\..\bin\elenart60.cfg %~dp0\output32\bin
copy %~dp0\..\bin\elenavm60.cfg %~dp0\output32\bin
copy %~dp0\..\bin\elt60.es %~dp0\output32\bin
copy %~dp0\..\bin\command60.es %~dp0\output32\bin

copy %~dp0\..\bin\x32\core60.bin %~dp0\output32\bin\x32\
copy %~dp0\..\bin\x32\core60_win.bin %~dp0\output32\bin\x32\
copy %~dp0\..\bin\x32\corex60.bin %~dp0\output32\bin\x32\

copy %~dp0\..\bin\templates\*.cfg %~dp0\output32\bin\templates\
copy %~dp0\..\bin\scripts\*.es %~dp0\output32\bin\scripts\

copy %~dp0\..\doc\license %~dp0\output32\doc\
copy %~dp0\..\doc\contributors %~dp0\output32\doc\
copy %~dp0\..\readme.md %~dp0\output32\

md %~dp0\output32\src60\system
xcopy %~dp0\..\src60\system\*.l %~dp0\output32\src60\system /s
xcopy %~dp0\..\src60\system\*.prj %~dp0\output32\src60\system /s

md %~dp0\output32\src60\extensions
xcopy %~dp0\..\src60\extensions\*.l %~dp0\output32\src60\extensions /s
xcopy %~dp0\..\src60\extensions\*.prj %~dp0\output32\src60\extensions /s

md %~dp0\output32\src60\cellular
xcopy %~dp0\..\src60\cellular\*.l %~dp0\output32\src60\cellular /s
xcopy %~dp0\..\src60\cellular\*.prj %~dp0\output32\src60\cellular /s

%~dp0\..\bin\sg-cli.exe %~dp0\..\dat\sg\syntax60.txt
@echo off 
if %ERRORLEVEL% EQU -2 GOTO CompilerError
@echo on

move %~dp0..\dat\sg\syntax60.dat %~dp0\output32\bin\

%~dp0\..\bin\og-cli %~dp0\..\dat\og\bc_rules60.txt
%~dp0\..\bin\og-cli -s %~dp0\..\dat\og\bt_rules60.txt

move %~dp0..\dat\og\bt_rules60.dat %~dp0\output32\bin\
move %~dp0..\dat\og\bc_rules60.dat %~dp0\output32\bin\

md %~dp0\lib60

%~dp0\..\bin\asm-cli -bc32 %~dp0\..\src60\core\system.core_routines.esm %~dp0\output32\lib60
@echo off 
if %ERRORLEVEL% EQU -2 GOTO CompilerError
@echo on

%~dp0\..\bin\asm-cli -bc32 %~dp0\..\src60\core\system.win_core_routines.esm %~dp0\output32\lib60
@echo off 
if %ERRORLEVEL% EQU -2 GOTO CompilerError
@echo on

%~dp0\output32\bin\elena-cli %~dp0\output32\src60\system\system.prj
@echo off 
if %ERRORLEVEL% EQU -2 GOTO CompilerError
@echo on

%~dp0\output32\bin\elena-cli %~dp0\output32\src60\extensions\extensions.prj
@echo off 
if %ERRORLEVEL% EQU -2 GOTO CompilerError
@echo on

%~dp0\output32\bin\elena-cli %~dp0\output32\src60\cellular\cellular.prj
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

copy %~dp0\..\examples60\console\helloworld\*.l %~dp0\output32\examples60\console\helloworld
copy %~dp0\..\examples60\console\helloworld\*.prj %~dp0\output32\examples60\console\helloworld

copy %~dp0\..\examples60\console\sum\*.l %~dp0\output32\examples60\console\sum
copy %~dp0\..\examples60\console\sum\*.prj %~dp0\output32\examples60\console\sum

copy %~dp0\..\examples60\console\words\*.l %~dp0\output32\examples60\console\words
copy %~dp0\..\examples60\console\binary\*.l %~dp0\output32\examples60\console\binary
copy %~dp0\..\examples60\console\bsort\*.l %~dp0\output32\examples60\console\bsort
copy %~dp0\..\examples60\console\datetime\*.l %~dp0\output32\examples60\console\datetime
copy %~dp0\..\examples60\console\datetime\*.prj %~dp0\output32\examples60\console\datetime
copy %~dp0\..\examples60\console\goods\*.l %~dp0\output32\examples60\console\goods
copy %~dp0\..\examples60\console\goods\*.txt %~dp0\output32\examples60\console\goods
copy %~dp0\..\examples60\console\replace\*.l %~dp0\output32\examples60\console\replace
copy %~dp0\..\examples60\console\pi\*.l %~dp0\output32\examples60\console\pi

copy %~dp0\..\examples60\scripts\calc\*.l %~dp0\output32\examples60\scripts\calc
copy %~dp0\..\examples60\scripts\calc\*.es %~dp0\output32\examples60\scripts\calc
copy %~dp0\..\examples60\scripts\calc\*.prj %~dp0\output32\examples60\scripts\calc

copy %~dp0\..\examples60\rosetta\accumulator\*.l %~dp0\output32\examples60\rosetta\accumulator
copy %~dp0\..\examples60\rosetta\ackermann\*.l %~dp0\output32\examples60\rosetta\ackermann
copy %~dp0\..\examples60\rosetta\addfield\*.l %~dp0\output32\examples60\rosetta\addfield
copy %~dp0\..\examples60\rosetta\anonymrec\*.l %~dp0\output32\examples60\rosetta\anonymrec
copy %~dp0\..\examples60\rosetta\aplusb\*.l %~dp0\output32\examples60\rosetta\aplusb
copy %~dp0\..\examples60\rosetta\applycallback\*.l %~dp0\output32\examples60\rosetta\applycallback
copy %~dp0\..\examples60\rosetta\arithmeticint\*.l %~dp0\output32\examples60\rosetta\arithmeticint
copy %~dp0\..\examples60\rosetta\arithmeval\*.l %~dp0\output32\examples60\rosetta\arithmeval
copy %~dp0\..\examples60\rosetta\arithmmean\*.l %~dp0\output32\examples60\rosetta\arithmmean
copy %~dp0\..\examples60\rosetta\arrayconcat\*.l %~dp0\output32\examples60\rosetta\arrayconcat
copy %~dp0\..\examples60\rosetta\arraymode\*.l %~dp0\output32\examples60\rosetta\arraymode
copy %~dp0\..\examples60\rosetta\arrays\*.l %~dp0\output32\examples60\rosetta\arrays

copy %~dp0\..\examples60\rosetta\associativearrays\*.l %~dp0\output32\examples60\rosetta\associativearrays 
copy %~dp0\..\examples60\rosetta\bestshuffle\*.l %~dp0\output32\examples60\rosetta\bestshuffle
copy %~dp0\..\examples60\rosetta\binary\*.l %~dp0\output32\examples60\rosetta\binary
copy %~dp0\..\examples60\rosetta\bitwise\*.l %~dp0\output32\examples60\rosetta\bitwise
copy %~dp0\..\examples60\rosetta\brackets\*.l %~dp0\output32\examples60\rosetta\brackets
copy %~dp0\..\examples60\rosetta\bullscows\*.l %~dp0\output32\examples60\rosetta\bullscows
copy %~dp0\..\examples60\rosetta\caesar\*.l %~dp0\output32\examples60\rosetta\caesar
copy %~dp0\..\examples60\rosetta\calendar\*.l %~dp0\output32\examples60\rosetta\calendar
copy %~dp0\..\examples60\rosetta\charmatch\*.l %~dp0\output32\examples60\rosetta\charmatch
copy %~dp0\..\examples60\rosetta\combinations\*.l %~dp0\output32\examples60\rosetta\combinations
copy %~dp0\..\examples60\rosetta\doors\*.l %~dp0\output32\examples60\rosetta\doors
copy %~dp0\..\examples60\rosetta\evolutionary\*.l %~dp0\output32\examples60\rosetta\evolutionary 
copy %~dp0\..\examples60\rosetta\firstclass\*.l %~dp0\output32\examples60\rosetta\firstclass 
copy %~dp0\..\examples60\rosetta\loop_multiple_arrays\*.l %~dp0\output32\examples60\rosetta\loop_multiple_arrays
copy %~dp0\..\examples60\rosetta\median\*.l %~dp0\output32\examples60\rosetta\median
copy %~dp0\..\examples60\rosetta\ninetynine\*.l %~dp0\output32\examples60\rosetta\ninetynine
copy %~dp0\..\examples60\rosetta\reverse_words_in_string\*.l %~dp0\output32\examples60\rosetta\reverse_words_in_string
copy %~dp0\..\examples60\rosetta\smavg\*.l %~dp0\output32\examples60\rosetta\smavg
copy %~dp0\..\examples60\rosetta\string_append\*.l %~dp0\output32\examples60\rosetta\string_append
copy %~dp0\..\examples60\rosetta\twentyfour\*.l %~dp0\output32\examples60\rosetta\twentyfour

7z a %~dp0\output32\%1.zip %~dp0\output32\*.* %~dp0\output32\* -r

goto:eof
::ERRORS
::---------------------
:CompilerError
echo The MSBuild returns error %ERRORLEVEL%
goto:eof
