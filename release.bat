echo compiling project..

%1\bin\sg %1\dat\sg\syntax.txt
move %1\dat\sg\syntax.dat %1\bin

%1\bin\og %1\dat\og\rules.txt
move %1\dat\og\rules.dat %1\bin

md lib30
md lib30\inline
%1\bin\asm2binx %1\src30\asm\core_routines.esm lib30\inline
%1\bin\asm2binx %1\src30\asm\ext_routines.esm lib30\inline

%1\bin\asm2binx %1\src30\asm\x32\commands.asm %1\bin\x32
%1\bin\asm2binx %1\src30\asm\x32\core.asm %1\bin\x32
%1\bin\asm2binx %1\src30\asm\x32\core_api.asm %1\bin\x32
%1\bin\asm2binx %1\src30\asm\x32\core_vm.asm %1\bin\x32

echo copying bin files..

md bin

copy %1\bin\asm2binx.exe bin
copy %1\bin\elc.exe bin
copy %1\bin\elide.exe bin
copy %1\bin\sg.exe bin
copy %1\bin\elt.exe bin
copy %1\bin\ecv.exe bin
copy %1\bin\og.exe bin

copy %1\bin\elenavm.dll bin
copy %1\bin\elenasm.dll bin
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

md bin\scripts
copy %1\bin\scripts\*.es bin\scripts\

echo copying dat files

md dat
md dat\sg
copy %1\dat\sg\sg.bat dat\sg
copy %1\dat\sg\syntax.txt dat\sg

md dat\og
copy %1\dat\og\rules.txt dat\og

echo copying doc files

md doc
copy %1\doc\knownbugs.txt doc\
copy %1\doc\license.txt doc\
copy %1\doc\roadmap.txt doc\
copy %1\doc\todo.txt doc\

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

md elenasrc2\elc\linux32
copy %1\elenasrc2\elc\linux32\*.cpp elenasrc2\elc\linux32

md elenasrc2\elc\vs
copy %1\elenasrc2\elc\vs\elc10.vcxproj elenasrc2\elc\vs
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
copy %1\elenasrc2\elenavm\vs\elenavm.vcxproj elenasrc2\elenavm\vs
copy %1\elenasrc2\elenavm\vs\elenavm11.vcxproj elenasrc2\elenavm\vs

md elenasrc2\elenavm\win32 
copy %1\elenasrc2\elenavm\win32\*.cpp elenasrc2\elenavm\win32
copy %1\elenasrc2\elenavm\win32\*.h elenasrc2\elenavm\win32

md elenasrc2\engine
copy %1\elenasrc2\engine\*.cpp elenasrc2\engine
copy %1\elenasrc2\engine\*.h elenasrc2\engine

md elenasrc2\engine\win32 
copy %1\elenasrc2\engine\win32\*.cpp elenasrc2\engine\win32
copy %1\elenasrc2\engine\win32\*.h elenasrc2\engine\win32

md elenasrc2\gui
copy %1\elenasrc2\gui\*.cpp elenasrc2\gui
copy %1\elenasrc2\gui\*.h elenasrc2\gui

md elenasrc2\gui\gtk-linux32
copy %1\elenasrc2\gui\gtk-linux32\*.cpp elenasrc2\gui\gtk-linux32
copy %1\elenasrc2\gui\gtk-linux32\*.h elenasrc2\gui\gtk-linux32

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

md elenasrc2\ide\gtk-linux32
copy %1\elenasrc2\ide\gtk-linux32\*.cpp elenasrc2\ide\gtk-linux32
copy %1\elenasrc2\ide\gtk-linux32\*.h elenasrc2\ide\gtk-linux32

md elenasrc2\ide\vs
copy %1\elenasrc2\ide\vs\elide10.vcxproj elenasrc2\ide\vs
copy %1\elenasrc2\ide\vs\elide11.vcxproj elenasrc2\ide\vs

md elenasrc2\ide\winapi32 
copy %1\elenasrc2\ide\winapi32\*.cpp elenasrc2\ide\winapi32 
copy %1\elenasrc2\ide\winapi32\*.h elenasrc2\ide\winapi32 
copy %1\elenasrc2\ide\winapi32\*.rc elenasrc2\ide\winapi32 

md elenasrc2\ide\winapi32\icons
copy %1\elenasrc2\ide\winapi32\icons\*.bmp elenasrc2\ide\winapi32\icons 
copy %1\elenasrc2\ide\winapi32\icons\*.ico elenasrc2\ide\winapi32\icons 

md elenasrc2\elenasm 
copy %1\elenasrc2\elenasm\*.cpp elenasrc2\elenasm
copy %1\elenasrc2\elenasm\*.h elenasrc2\elenasm

md elenasrc2\elenasm\win32
copy %1\elenasrc2\elenasm\win32\*.cpp elenasrc2\elenasm\win32
copy %1\elenasrc2\elenasm\win32\*.h elenasrc2\elenasm\win32

md elenasrc2\elenasm\vs
copy %1\elenasrc2\elenasm\vs\*.vcxproj elenasrc2\elenasm\vs

md elenasrc2\elenasm\codeblocks 
copy %1\elenasrc2\elenasm\codeblocks\*.cbp elenasrc2\elenasm\codeblocks

md elenasrc2\tools 

md elenasrc2\tools\api2html
copy %1\elenasrc2\tools\api2html\*.cpp elenasrc2\tools\api2html
copy %1\elenasrc2\tools\api2html\*.h elenasrc2\tools\api2html

md elenasrc2\tools\api2html\codeblocks 
copy %1\elenasrc2\tools\api2html\codeblocks\*.cbp elenasrc2\tools\api2html\codeblocks

md elenasrc2\tools\api2html\vs
copy %1\elenasrc2\tools\api2html\vs\*.vcxproj elenasrc2\tools\api2html\vs

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
md elenasrc2\tools\sg\linux32
copy %1\elenasrc2\tools\sg\win32\*.h elenasrc2\tools\sg\win32
copy %1\elenasrc2\tools\sg\linux32\*.h elenasrc2\tools\sg\linux32

md elenasrc2\tools\sg\codeblocks 
copy %1\elenasrc2\tools\sg\codeblocks\*.cbp elenasrc2\tools\sg\codeblocks

md elenasrc2\tools\sg\vs
copy %1\elenasrc2\tools\sg\vs\*.vcxproj elenasrc2\tools\sg\vs

md elenasrc2\tools\elt
copy %1\elenasrc2\tools\elt\*.cpp elenasrc2\tools\elt
copy %1\elenasrc2\tools\elt\*.h elenasrc2\tools\elt

md elenasrc2\tools\elt\vs
copy %1\elenasrc2\tools\elt\vs\*.vcxproj elenasrc2\tools\elt\vs

md elenasrc2\tools\elt\codeblocks 
copy %1\elenasrc2\tools\elt\codeblocks\*.cbp elenasrc2\tools\elt\codeblocks

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

md examples\c_a_g
copy %1\examples\c_a_g\*.l examples\c_a_g
copy %1\examples\c_a_g\*.prj examples\c_a_g

md examples\c_a_g\formulas

md examples\c_a_g\formulas\Circulo
copy %1\examples\c_a_g\formulas\Circulo\*.bmp examples\c_a_g\formulas\Circulo

md examples\c_a_g\formulas\Paralelogramos 
copy %1\examples\c_a_g\formulas\Paralelogramos\*.bmp examples\c_a_g\formulas\Paralelogramos 

md examples\c_a_g\formulas\Trapezio 
copy %1\examples\c_a_g\formulas\Trapezio\*.bmp examples\c_a_g\formulas\Trapezio

md examples\c_a_g\formulas\Triangulos 
copy %1\examples\c_a_g\formulas\Triangulos\*.bmp examples\c_a_g\formulas\Triangulos 

md examples\c_a_g\inf
md examples\c_a_g\obj
md examples\c_a_g\bin

md examples\calculator 
copy %1\examples\calculator\*.l examples\calculator 
copy %1\examples\calculator\*.prj examples\calculator 
copy %1\examples\calculator\calc.vl2 examples\calculator 
copy %1\examples\calculator\calc.vl examples\calculator 

md examples\datetime 
copy %1\examples\datetime\*.l examples\datetime 
copy %1\examples\datetime\*.prj examples\datetime  

md examples\dices
copy %1\examples\dices\*.l examples\dices
copy %1\examples\dices\*.prj examples\dices

md examples\dices\kniffel
copy %1\examples\dices\kniffel\*.l examples\dices\kniffel

md examples\eldoc 
copy %1\examples\eldoc\*.l examples\eldoc
copy %1\examples\eldoc\*.prj examples\eldoc

md examples\goods 
copy %1\examples\goods\*.l examples\goods 
copy %1\examples\goods\*.txt examples\goods 
copy %1\examples\goods\*.prj examples\goods 

md examples\graphs
copy %1\examples\graphs\*.l examples\graphs
copy %1\examples\graphs\*.prj examples\graphs

md examples\helloworld 
copy %1\examples\helloworld\*.l examples\helloworld
copy %1\examples\helloworld\*.prj examples\helloworld
copy %1\examples\helloworld\*.es examples\helloworld

md examples\interpreter
copy %1\examples\interpreter\*.l examples\interpreter
copy %1\examples\interpreter\*.prj examples\interpreter
copy %1\examples\interpreter\*.txt examples\interpreter
copy %1\examples\interpreter\*.bat examples\interpreter
copy %1\examples\interpreter\*.vl2 examples\interpreter

md examples\matrix 
copy %1\examples\matrix\*.l examples\matrix 
copy %1\examples\matrix\*.prj examples\matrix 

md examples\opencalc
copy %1\examples\opencalc\*.bat examples\opencalc
copy %1\examples\opencalc\*.vl examples\opencalc

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

md examples\upndown 
md examples\upndown\bin
copy %1\examples\upndown\*.l examples\upndown 
copy %1\examples\upndown\*.prj examples\upndown 

md examples\upndown\gui
copy %1\examples\upndown\gui\*.l examples\upndown\gui

md examples\upndown\upndown
copy %1\examples\upndown\upndown\*.l examples\upndown\upndown 

md examples\upndown\dictionary
copy %1\examples\upndown\dictionary\*.l examples\upndown\dictionary

md examples\notepad 
md examples\notepad\bin
copy %1\examples\notepad\*.l examples\notepad
copy %1\examples\notepad\*.prj examples\notepad 

md examples\trans
copy %1\examples\trans\*.l examples\trans
copy %1\examples\trans\*.prj examples\trans

md examples\vmconsole
copy %1\examples\vmconsole\*.l examples\vmconsole
copy %1\examples\vmconsole\*.prj examples\vmconsole

md examples\timer
copy %1\examples\timer\*.l examples\timer
copy %1\examples\timer\*.prj examples\timer

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

copy %1\rebuild.bat 
copy %1\rebuildall.bat 
copy %1\*.txt

rebuild.bat 
