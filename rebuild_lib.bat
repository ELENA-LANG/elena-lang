bin\sg dat\sg\syntax.txt
move dat\sg\syntax.dat bin

bin\og dat\og\rules.txt
move dat\og\rules.dat bin

bin\og s dat\og\source_rules.txt 
move dat\og\source_rules.dat bin

bin\asm2binx src50\core\system.core_routines.esm lib50
@echo off 
if %ERRORLEVEL% EQU -1 GOTO Asm2BinError
@echo on

bin\asm2binx asm\x32\core.asm bin\x32
@echo off 
if %ERRORLEVEL% EQU -1 GOTO Asm2BinError
@echo on

bin\asm2binx asm\x32\corex.asm bin\x32                
@echo off 
if %ERRORLEVEL% EQU -1 GOTO Asm2BinError
@echo on

bin\asm2binx asm\x32\coreapi.asm bin\x32
@echo off 
if %ERRORLEVEL% EQU -1 GOTO Asm2BinError
@echo on

bin\asm2binx asm\x32\core_win.asm bin\x32
@echo off 
if %ERRORLEVEL% EQU -1 GOTO Asm2BinError
@echo on

rem bin\asm2binx -amd64 asm\amd64\core.asm bin\amd64
rem bin\asm2binx -amd64 asm\amd64\core_win.asm bin\amd64
rem bin\asm2binx -amd64 asm\amd64\coreapi.asm bin\amd64

bin\elc src40\system\system.prj
@echo off 
if %ERRORLEVEL% EQU -2 GOTO CompilerError
@echo on

rem bin\elc src40\extensions\extensions.prj
rem @echo off 
rem if %ERRORLEVEL% EQU -2 GOTO CompilerError
rem @echo on

rem bin\elc src40\net\net.prj
rem @echo off 
rem if %ERRORLEVEL% EQU -2 GOTO CompilerError
rem @echo on

rem bin\elc src40\forms\forms.prj
rem @echo off 
rem if %ERRORLEVEL% EQU -2 GOTO CompilerError
rem @echo on

rem bin\elc src40\sqlite\sqlite.prj
rem @echo off 
rem if %ERRORLEVEL% EQU -2 GOTO CompilerError
rem @echo on

rem bin\elc src40\cellular\cellular.prj
rem @echo off 
rem if %ERRORLEVEL% EQU -2 GOTO CompilerError
rem @echo on

rem bin\elc src40\algorithms\algorithms.prj
rem @echo off 
rem if %ERRORLEVEL% EQU -2 GOTO CompilerError
rem @echo on

rem bin\elc src40\graphics\graphics.prj
rem bin\elc src40\xforms\xforms.prj

rem bin\elc src40\system\system_64.prj

echo Done

@echo off 
goto:eof
@echo on

:Asm2BinError
echo ASM2BINX returns error %ERRORLEVEL%
@echo off
goto:eof
@echo on

:CompilerError
echo ELC returns error %ERRORLEVEL%
@echo off
goto:eof
@echo on
