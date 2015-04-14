@echo off

reg.exe query "HKLM\SOFTWARE\Microsoft\MSBuild\ToolsVersions\12.0" /v MSBuildToolsPath > nul 2>&1
if ERRORLEVEL 1 goto MissingMSBuildRegistry

for /f "skip=2 tokens=2,*" %%A in ('reg.exe query "HKLM\SOFTWARE\Microsoft\MSBuild\ToolsVersions\4.0" /v MSBuildToolsPath') do SET MSBUILDDIR=%%B

IF NOT EXIST %MSBUILDDIR%nul goto MissingMSBuildToolsPath
IF NOT EXIST %MSBUILDDIR%msbuild.exe goto MissingMSBuildExe

ECHO =========== Starting Release Compile ==================

ECHO Command line Compiler compiling....
ECHO -----------------------------------
"%MSBUILDDIR%msbuild.exe" %1\elenasrc2\elc\vs\elc11.vcxproj /p:configuration=release
IF NOT %ERRORLEVEL%==0 GOTO CompilerError

ECHO Virtual Machine compiling....
ECHO -----------------------------
"%MSBUILDDIR%msbuild.exe" %1\elenasrc2\elenavm\vs\elenavm11.vcxproj /p:configuration=release
IF NOT %ERRORLEVEL%==0 GOTO CompilerError

ECHO IDE compiling....
ECHO -----------------
"%MSBUILDDIR%msbuild.exe" %1\elenasrc2\ide\vs\elide11.vcxproj /p:configuration=release
IF NOT %ERRORLEVEL%==0 GOTO CompilerError

ECHO Run-Time Engine compiling....
ECHO -----------------------------
"%MSBUILDDIR%msbuild.exe" %1\elenasrc2\elenart\vs\elenart.vcxproj /p:configuration=release
IF NOT %ERRORLEVEL%==0 GOTO CompilerError

ECHO Script Engine compiling....
ECHO ----------------------------
rem "%MSBUILDDIR%msbuild.exe" %1\elenasrc2\elenasm\vs\elenasm11.vcxproj /p:configuration=release
rem IF NOT %ERRORLEVEL%==0 GOTO CompilerError

ECHO Simplified Assembler compiling....
ECHO -----------------------------------
"%MSBUILDDIR%msbuild.exe" %1\elenasrc2\tools\asm2bin\vs\asm2binx11.vcxproj /p:configuration=release
IF NOT %ERRORLEVEL%==0 GOTO CompilerError

ECHO ECODES viewer compiling....
ECHO ---------------------------
"%MSBUILDDIR%msbuild.exe" %1\elenasrc2\tools\ecv\vs\ecv11.vcxproj /p:configuration=release
IF NOT %ERRORLEVEL%==0 GOTO CompilerError

ECHO Virtual Machine Terminal compiling....
ECHO --------------------------------------
rem "%MSBUILDDIR%msbuild.exe" %1\elenasrc2\tools\elt\vs\elt11.vcxproj /p:configuration=release
rem IF NOT %ERRORLEVEL%==0 GOTO CompilerError

ECHO Optimization Rule Generator compiling....
ECHO ------------------------------------------
"%MSBUILDDIR%msbuild.exe" %1\elenasrc2\tools\og\vs\og11.vcxproj /p:configuration=release
IF NOT %ERRORLEVEL%==0 GOTO CompilerError

ECHO Syntax Parse Table Generator compiling....
ECHO ------------------------------------------
"%MSBUILDDIR%msbuild.exe" %1\elenasrc2\tools\sg\vs\sg10.vcxproj /p:configuration=release
IF NOT %ERRORLEVEL%==0 GOTO CompilerError

ECHO =========== Release Compiled ==================

ECHO =========== Compiling ELENA files ==================

%1\bin\sg %1\dat\sg\syntax.txt
move %1\dat\sg\syntax.dat %1\bin

%1\bin\og %1\dat\og\rules.txt
move %1\dat\og\rules.dat %1\bin

md lib30
%1\bin\asm2binx %1\src30\asm\core_routines.esm lib30\system
%1\bin\asm2binx %1\src30\asm\ext_routines.esm lib30\system

%1\bin\asm2binx %1\src30\asm\x32\commands.asm %1\bin\x32
%1\bin\asm2binx %1\src30\asm\x32\core.asm %1\bin\x32
%1\bin\asm2binx %1\src30\asm\x32\coreapi.asm %1\bin\x32
%1\bin\asm2binx %1\src30\asm\x32\core_vm.asm %1\bin\x32
%1\bin\asm2binx %1\src30\asm\x32\core_rt.asm %1\bin\x32
%1\bin\asm2binx %1\src30\asm\x32\core_win.asm %1\bin\x32

echo ========== copying bin files.. ===============

md bin

copy %1\bin\asm2binx.exe bin
copy %1\bin\elc.exe bin
copy %1\bin\elide.exe bin
copy %1\bin\sg.exe bin
rem copy %1\bin\elt.exe bin
copy %1\bin\ecv.exe bin
copy %1\bin\og.exe bin

copy %1\bin\elenavm.dll bin
copy %1\bin\elenart.dll bin
rem copy %1\bin\elenasm.dll bin
copy %1\bin\winstub.ex_ bin
copy %1\bin\syntax.dat bin
copy %1\bin\rules.dat bin
copy %1\bin\elc.cfg bin
copy %1\bin\elenavm.cfg bin
copy %1\bin\elt.cfg bin

echo copying bin\templates files

md bin\templates
copy %1\bin\templates\*.cfg bin\templates\

echo copying bin\x32 files

md bin\x32
copy %1\bin\x32\*.bin bin\x32\

echo copying bin\scripts files

rem md bin\scripts
rem copy %1\bin\scripts\*.es bin\scripts\

echo copying dat files

md dat
md dat\sg
copy %1\dat\sg\syntax.txt dat\sg

md dat\og
copy %1\dat\og\rules.txt dat\og

echo copying doc files

md doc
copy %1\doc\license.txt doc\
copy %1\doc\roadmap.txt doc\

md doc\api
copy %1\doc\api\*.html doc\api

md doc\lang
copy %1\doc\lang\dictionary.txt  doc\lang

md doc\tech 
copy %1\doc\tech\bytecode.txt doc\tech 
copy %1\doc\tech\knowhow.txt doc\tech 

echo copying elenasrc2 files

md elenasrc2
copy %1\elenasrc2\elena2.workspace elenasrc2

md elenasrc2\common
copy %1\elenasrc2\common\*.cpp elenasrc2\common
copy %1\elenasrc2\common\*.h elenasrc2\common

md elenasrc2\elc
copy %1\elenasrc2\elc\*.cpp elenasrc2\elc
copy %1\elenasrc2\elc\*.h elenasrc2\elc

md elenasrc2\elc\codeblocks 
copy %1\elenasrc2\elc\codeblocks\*.cbp elenasrc2\elc\codeblocks

md elenasrc2\elc\vs
copy %1\elenasrc2\elc\vs\elc11.vcxproj elenasrc2\elc\vs

md elenasrc2\elc\win32 
copy %1\elenasrc2\elc\win32\*.cpp elenasrc2\elc\win32
copy %1\elenasrc2\elc\win32\*.h elenasrc2\elc\win32

md elenasrc2\elenavm
copy %1\elenasrc2\elenavm\*.cpp elenasrc2\elenavm
copy %1\elenasrc2\elenavm\*.h elenasrc2\elenavm

md elenasrc2\elenavm\codeblocks 
copy %1\elenasrc2\elenavm\codeblocks\*.cbp elenasrc2\elenavm\codeblocks

md elenasrc2\elenavm\vs
copy %1\elenasrc2\elenavm\vs\elenavm11.vcxproj elenasrc2\elenavm\vs

md elenasrc2\elenavm\win32 
copy %1\elenasrc2\elenavm\win32\*.cpp elenasrc2\elenavm\win32
copy %1\elenasrc2\elenavm\win32\*.h elenasrc2\elenavm\win32

md elenasrc2\elenart
copy %1\elenasrc2\elenart\*.cpp elenasrc2\elenart
copy %1\elenasrc2\elenart\*.h elenasrc2\elenart

md elenasrc2\elenart\codeblocks 
copy %1\elenasrc2\elenart\codeblocks\*.cbp elenasrc2\elenart\codeblocks

md elenasrc2\elenart\vs
copy %1\elenasrc2\elenart\vs\elenart.vcxproj elenasrc2\elenart\vs

md elenasrc2\elenart\win32 
copy %1\elenasrc2\elenart\win32\*.cpp elenasrc2\elenart\win32
copy %1\elenasrc2\elenart\win32\*.h elenasrc2\elenart\win32

md elenasrc2\engine
copy %1\elenasrc2\engine\*.cpp elenasrc2\engine
copy %1\elenasrc2\engine\*.h elenasrc2\engine

md elenasrc2\engine\win32 
copy %1\elenasrc2\engine\win32\*.cpp elenasrc2\engine\win32
copy %1\elenasrc2\engine\win32\*.h elenasrc2\engine\win32

md elenasrc2\gui
copy %1\elenasrc2\gui\*.cpp elenasrc2\gui

md elenasrc2\gui\winapi32 
copy %1\elenasrc2\gui\winapi32\*.cpp elenasrc2\gui\winapi32 
copy %1\elenasrc2\gui\winapi32\*.h elenasrc2\gui\winapi32 

md elenasrc2\ide
copy %1\elenasrc2\ide\*.cpp elenasrc2\ide
copy %1\elenasrc2\ide\*.h elenasrc2\ide

md elenasrc2\ide\codeblocks 
copy %1\elenasrc2\ide\codeblocks\*.cbp elenasrc2\ide\codeblocks

md elenasrc2\ide\eng
copy %1\elenasrc2\ide\eng\*.h elenasrc2\ide\eng

md elenasrc2\ide\vs
copy %1\elenasrc2\ide\vs\elide11.vcxproj elenasrc2\ide\vs

md elenasrc2\ide\winapi32 
copy %1\elenasrc2\ide\winapi32\*.cpp elenasrc2\ide\winapi32 
copy %1\elenasrc2\ide\winapi32\*.h elenasrc2\ide\winapi32 
copy %1\elenasrc2\ide\winapi32\*.rc elenasrc2\ide\winapi32 

md elenasrc2\ide\winapi32\icons
copy %1\elenasrc2\ide\winapi32\icons\*.bmp elenasrc2\ide\winapi32\icons 
copy %1\elenasrc2\ide\winapi32\icons\*.ico elenasrc2\ide\winapi32\icons 

rem md elenasrc2\elenasm 
rem copy %1\elenasrc2\elenasm\*.cpp elenasrc2\elenasm
rem copy %1\elenasrc2\elenasm\*.h elenasrc2\elenasm

rem md elenasrc2\elenasm\win32
rem copy %1\elenasrc2\elenasm\win32\*.cpp elenasrc2\elenasm\win32
rem copy %1\elenasrc2\elenasm\win32\*.h elenasrc2\elenasm\win32

rem md elenasrc2\elenasm\vs
rem copy %1\elenasrc2\elenasm\vs\*.vcxproj elenasrc2\elenasm\vs

rem md elenasrc2\elenasm\codeblocks 
rem copy %1\elenasrc2\elenasm\codeblocks\*.cbp elenasrc2\elenasm\codeblocks

md elenasrc2\tools 

rem md elenasrc2\tools\api2html
rem copy %1\elenasrc2\tools\api2html\*.cpp elenasrc2\tools\api2html
rem copy %1\elenasrc2\tools\api2html\*.h elenasrc2\tools\api2html

rem md elenasrc2\tools\api2html\codeblocks 
rem copy %1\elenasrc2\tools\api2html\codeblocks\*.cbp elenasrc2\tools\api2html\codeblocks

rem md elenasrc2\tools\api2html\vs
rem copy %1\elenasrc2\tools\api2html\vs\*.vcxproj elenasrc2\tools\api2html\vs

md elenasrc2\tools\asm2bin
copy %1\elenasrc2\tools\asm2bin\*.cpp elenasrc2\tools\asm2bin
copy %1\elenasrc2\tools\asm2bin\*.h elenasrc2\tools\asm2bin

md elenasrc2\tools\asm2bin\codeblocks 
copy %1\elenasrc2\tools\asm2bin\codeblocks\*.cbp elenasrc2\tools\asm2bin\codeblocks

md elenasrc2\tools\asm2bin\vs
copy %1\elenasrc2\tools\asm2bin\vs\*.vcxproj elenasrc2\tools\asm2bin\vs

md elenasrc2\tools\sg
copy %1\elenasrc2\tools\sg\*.cpp elenasrc2\tools\sg
copy %1\elenasrc2\tools\sg\*.h elenasrc2\tools\sg
md elenasrc2\tools\sg\win32
copy %1\elenasrc2\tools\sg\win32\*.h elenasrc2\tools\sg\win32

md elenasrc2\tools\sg\codeblocks 
copy %1\elenasrc2\tools\sg\codeblocks\*.cbp elenasrc2\tools\sg\codeblocks

md elenasrc2\tools\sg\vs
copy %1\elenasrc2\tools\sg\vs\*.vcxproj elenasrc2\tools\sg\vs

rem md elenasrc2\tools\elt
rem copy %1\elenasrc2\tools\elt\*.cpp elenasrc2\tools\elt
rem copy %1\elenasrc2\tools\elt\*.h elenasrc2\tools\elt

rem md elenasrc2\tools\elt\vs
rem copy %1\elenasrc2\tools\elt\vs\*.vcxproj elenasrc2\tools\elt\vs

rem md elenasrc2\tools\elt\codeblocks 
rem copy %1\elenasrc2\tools\elt\codeblocks\*.cbp elenasrc2\tools\elt\codeblocks

md elenasrc2\tools\ecv
copy %1\elenasrc2\tools\ecv\*.cpp elenasrc2\tools\ecv
copy %1\elenasrc2\tools\ecv\*.h elenasrc2\tools\ecv

md elenasrc2\tools\ecv\vs
copy %1\elenasrc2\tools\ecv\vs\*.vcxproj elenasrc2\tools\ecv\vs

md elenasrc2\tools\ecv\codeblocks 
copy %1\elenasrc2\tools\ecv\codeblocks\*.cbp elenasrc2\tools\ecv\codeblocks

md elenasrc2\tools\og
copy %1\elenasrc2\tools\og\*.cpp elenasrc2\tools\og
copy %1\elenasrc2\tools\og\*.h elenasrc2\tools\og

md elenasrc2\tools\og\codeblocks 
copy %1\elenasrc2\tools\og\codeblocks\*.cbp elenasrc2\tools\og\codeblocks

md elenasrc2\tools\og\vs
copy %1\elenasrc2\tools\og\vs\*.vcxproj elenasrc2\tools\og\vs

md install
copy %1\install\*.iss install

echo copying examples files

md examples 

md examples\agenda
copy %1\examples\agenda\*.l examples\agenda
copy %1\examples\agenda\*.prj examples\agenda

md examples\binary 
copy %1\examples\binary\*.l examples\binary
copy %1\examples\binary\*.prj examples\binary

md examples\bsort
copy %1\examples\bsort\*.l examples\bsort
copy %1\examples\bsort\*.prj examples\bsort

rem md examples\c_a_g
rem copy %1\examples\c_a_g\*.l examples\c_a_g
rem copy %1\examples\c_a_g\*.prj examples\c_a_g

rem md examples\c_a_g\formulas

rem md examples\c_a_g\formulas\Circulo
rem copy %1\examples\c_a_g\formulas\Circulo\*.bmp examples\c_a_g\formulas\Circulo

rem md examples\c_a_g\formulas\Paralelogramos 
rem copy %1\examples\c_a_g\formulas\Paralelogramos\*.bmp examples\c_a_g\formulas\Paralelogramos 

rem md examples\c_a_g\formulas\Trapezio 
rem copy %1\examples\c_a_g\formulas\Trapezio\*.bmp examples\c_a_g\formulas\Trapezio

rem md examples\c_a_g\formulas\Triangulos 
rem copy %1\examples\c_a_g\formulas\Triangulos\*.bmp examples\c_a_g\formulas\Triangulos 

rem md examples\c_a_g\inf
rem md examples\c_a_g\obj
rem md examples\c_a_g\bin

rem md examples\calculator 
rem copy %1\examples\calculator\*.l examples\calculator 
rem copy %1\examples\calculator\*.prj examples\calculator 
rem copy %1\examples\calculator\calc.vl2 examples\calculator 
rem copy %1\examples\calculator\calc.vl examples\calculator 

md examples\datetime 
copy %1\examples\datetime\*.l examples\datetime 
copy %1\examples\datetime\*.prj examples\datetime  

rem md examples\dices
rem copy %1\examples\dices\*.l examples\dices
rem copy %1\examples\dices\*.prj examples\dices

rem md examples\dices\kniffel
rem copy %1\examples\dices\kniffel\*.l examples\dices\kniffel

rem md examples\eldoc 
rem copy %1\examples\eldoc\*.l examples\eldoc
rem copy %1\examples\eldoc\*.prj examples\eldoc

md examples\goods 
copy %1\examples\goods\*.l examples\goods 
copy %1\examples\goods\*.txt examples\goods 
copy %1\examples\goods\*.prj examples\goods 

rem md examples\graphs
rem copy %1\examples\graphs\*.l examples\graphs
rem copy %1\examples\graphs\*.prj examples\graphs

md examples\helloworld 
copy %1\examples\helloworld\*.l examples\helloworld
copy %1\examples\helloworld\*.prj examples\helloworld
rem copy %1\examples\helloworld\*.es examples\helloworld

rem md examples\interpreter
rem copy %1\examples\interpreter\*.l examples\interpreter
rem copy %1\examples\interpreter\*.prj examples\interpreter
rem copy %1\examples\interpreter\*.txt examples\interpreter
rem copy %1\examples\interpreter\*.bat examples\interpreter
rem copy %1\examples\interpreter\*.es examples\interpreter

md examples\matrix 
copy %1\examples\matrix\*.l examples\matrix 
copy %1\examples\matrix\*.prj examples\matrix 

rem md examples\opencalc
rem copy %1\examples\opencalc\*.bat examples\opencalc
rem copy %1\examples\opencalc\*.vl examples\opencalc

md examples\pi
copy %1\examples\pi\*.l examples\pi
copy %1\examples\pi\*.prj examples\pi

md examples\replace
copy %1\examples\replace\*.l examples\replace
copy %1\examples\replace\*.prj examples\replace

md examples\sum
copy %1\examples\sum\*.l examples\sum
copy %1\examples\sum\*.prj examples\sum

md examples\textdb
copy %1\examples\textdb\*.l examples\textdb
copy %1\examples\textdb\*.prj examples\textdb
copy %1\examples\textdb\*.txt examples\textdb

md examples\textfile
copy %1\examples\textfile\*.l examples\textfile
copy %1\examples\textfile\*.prj examples\textfile
copy %1\examples\textfile\*.txt examples\textfile

md examples\words
copy %1\examples\words\*.l examples\words
copy %1\examples\words\*.prj examples\words

rem md examples\upndown 
rem md examples\upndown\bin
rem copy %1\examples\upndown\*.l examples\upndown 
rem copy %1\examples\upndown\*.prj examples\upndown 

rem md examples\upndown\gui
rem copy %1\examples\upndown\gui\*.l examples\upndown\gui

rem md examples\upndown\upndown
rem copy %1\examples\upndown\upndown\*.l examples\upndown\upndown 

rem md examples\upndown\dictionary
rem copy %1\examples\upndown\dictionary\*.l examples\upndown\dictionary

rem md examples\notepad 
rem md examples\notepad\bin
rem copy %1\examples\notepad\*.l examples\notepad
rem copy %1\examples\notepad\*.prj examples\notepad 

md examples\trans
copy %1\examples\trans\*.l examples\trans
copy %1\examples\trans\*.prj examples\trans

rem md examples\vmconsole
rem copy %1\examples\vmconsole\*.l examples\vmconsole
rem copy %1\examples\vmconsole\*.prj examples\vmconsole

rem md examples\timer
rem copy %1\examples\timer\*.l examples\timer
rem copy %1\examples\timer\*.prj examples\timer

echo copying src3 files

md src30
copy %1\src30\*.l src27

md src30\asm
md src30\asm\x32
copy %1\src30\asm\x32\*.asm src30\asm\x32
copy %1\src30\asm\*.esm src30\asm

md src30\system
xcopy %1\src30\system\*.l src30\system /s
xcopy %1\src30\system\*.prj src30\system /s

md src30\extensions
xcopy %1\src30\extensions\*.l src30\extensions /s
xcopy %1\src30\extensions\*.prj src30\extensions /s

md src30\forms
xcopy %1\src30\forms\*.l src30\forms /s
xcopy %1\src30\forms\*.prj src30\forms /s

copy %1\rebuild.bat 
copy %1\rebuildall.bat 
copy %1\*.txt

echo copying rosetta files

md rosetta
md rosetta\accumulator
copy %1\rosetta\accumulator\*.l rosetta\accumulator
copy %1\rosetta\accumulator\*.prj rosetta\accumulator

md rosetta\ackermann 
copy %1\rosetta\ackermann\*.l rosetta\ackermann
copy %1\rosetta\ackermann\*.prj rosetta\ackermann

md rosetta\addfield
copy %1\rosetta\addfield\*.l rosetta\addfield 
copy %1\rosetta\addfield\*.prj rosetta\addfield 

md rosetta\amb 
copy %1\rosetta\amb\*.l rosetta\amb
copy %1\rosetta\amb\*.prj rosetta\amb

md rosetta\aplusb
copy %1\rosetta\aplusb\*.l rosetta\aplusb
copy %1\rosetta\aplusb\*.prj rosetta\aplusb

md rosetta\applycallback 
copy %1\rosetta\applycallback\*.l rosetta\applycallback 
copy %1\rosetta\applycallback\*.prj rosetta\applycallback 

md rosetta\arithmeticint
copy %1\rosetta\arithmeticint\*.l rosetta\arithmeticint
copy %1\rosetta\arithmeticint\*.prj rosetta\arithmeticint

md rosetta\arithmmean
copy %1\rosetta\arithmmean\*.l rosetta\arithmmean 
copy %1\rosetta\arithmmean\*.prj rosetta\arithmmean 

md rosetta\associativearrays
copy %1\rosetta\associativearrays\*.l rosetta\associativearrays
copy %1\rosetta\associativearrays\*.prj rosetta\associativearrays

md rosetta\arithmeval
copy %1\rosetta\arithmeval\*.l rosetta\arithmeval
copy %1\rosetta\arithmeval\*.prj rosetta\arithmeval

md rosetta\arrays 
copy %1\arrays\*.l rosetta\arrays
copy %1\arrays\*.prj rosetta\arrays

md rosetta\arrayconcat
copy %1\rosetta\arrayconcat\*.l rosetta\arrayconcat
copy %1\rosetta\arrayconcat\*.prj rosetta\arrayconcat

md rosetta\smavg 
copy %1\rosetta\smavg\*.l rosetta\smavg 
copy %1\rosetta\smavg\*.prj rosetta\smavg 

md rosetta\arraymode
copy %1\rosetta\arraymode\*.l rosetta\arraymode
copy %1\rosetta\arraymode\*.prj rosetta\arraymode

md rosetta\anonymrec
copy %1\rosetta\anonymrec\*.l rosetta\anonymrec
copy %1\rosetta\anonymrec\*.prj rosetta\anonymrec

md rosetta\median
copy %1\rosetta\median\*.l rosetta\median
copy %1\rosetta\median\*.prj rosetta\median

md rosetta\bitwise
copy %1\rosetta\bitwise\*.l rosetta\bitwise
copy %1\rosetta\bitwise\*.prj rosetta\bitwise

md rosetta\anagram
copy %1\rosetta\anagram\*.l rosetta\anagram
copy %1\rosetta\anagram\*.prj rosetta\anagram
copy %1\rosetta\anagram\*.txt rosetta\anagram

md rosetta\brackets
copy %1\rosetta\brackets\*.l rosetta\brackets
copy %1\rosetta\brackets\*.prj rosetta\brackets

md rosetta\bestshuffle 
copy %1\rosetta\bestshuffle\*.l rosetta\bestshuffle
copy %1\rosetta\bestshuffle\*.prj rosetta\bestshuffle

md rosetta\bullscows
copy %1\rosetta\bullscows\*.l rosetta\bullscows
copy %1\rosetta\bullscows\*.prj rosetta\bullscows

md rosetta\binary
copy %1\rosetta\binary\*.l rosetta\binary
copy %1\rosetta\binary\*.prj rosetta\binary

md rosetta\caesar
copy %1\rosetta\caesar\*.l rosetta\caesar
copy %1\rosetta\caesar\*.prj rosetta\caesar

md rosetta\charmatch
copy %1\rosetta\charmatch\*.l rosetta\charmatch
copy %1\rosetta\charmatch\*.prj rosetta\charmatch

md rosetta\combinations
copy %1\rosetta\combinations\*.l rosetta\combinations
copy %1\rosetta\combinations\*.prj rosetta\combinations

md rosetta\calendar
copy %1\rosetta\calendar\*.l rosetta\calendar
copy %1\rosetta\calendar\*.prj rosetta\calendar

rebuild.bat 

goto:eof
::ERRORS
::---------------------
:MissingMSBuildRegistry
echo Cannot obtain path to MSBuild tools from registry
goto:eof
:MissingMSBuildToolsPath
echo The MSBuild tools path from the registry '%MSBUILDDIR%' does not exist
goto:eof
:MissingMSBuildExe
echo The MSBuild executable could not be found at '%MSBUILDDIR%'
goto:eof
:CompilerError
echo The MSBuild returns error %ERRORLEVEL%
goto:eof
