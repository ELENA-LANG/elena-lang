bin\sg dat\sg\syntax.txt
move dat\sg\syntax.dat bin

bin\og dat\og\rules.txt
move dat\og\rules.dat bin

bin\og s dat\og\source_rules.txt 
@echo off 
if %ERRORLEVEL% EQU -1 GOTO Asm2BinError
@echo on
move dat\og\source_rules.dat bin

bin\asm2binx src50\core\system.core_routines.esm lib50
@echo off 
if %ERRORLEVEL% EQU -1 GOTO Asm2BinError
@echo on

bin\asm2binx -p64 src50\core\system.core_routines.esm lib50_64 
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

bin\asm2binx -amd64 asm\amd64\core.asm bin\amd64
@echo off 
if %ERRORLEVEL% EQU -1 GOTO Asm2BinError
@echo on

bin\asm2binx -amd64 asm\amd64\core_win.asm bin\amd64
@echo off 
if %ERRORLEVEL% EQU -1 GOTO Asm2BinError
@echo on

bin\asm2binx -amd64 asm\amd64\coreapi.asm bin\amd64
@echo off 
if %ERRORLEVEL% EQU -1 GOTO Asm2BinError
@echo on

rem bin\asm2binx -ppc64le asm\ppc64le\core.asm bin\ppc64le

bin\elc src50\system\system.prj
@echo off 
if %ERRORLEVEL% EQU -2 GOTO CompilerError
@echo on

bin\elc src50\extensions\extensions.prj
@echo off 
if %ERRORLEVEL% EQU -2 GOTO CompilerError
@echo on

rem bin\elc src50\net\net.prj
@echo off 
if %ERRORLEVEL% EQU -2 GOTO CompilerError
@echo on

rem bin\elc src50\forms\forms.prj
@echo off 
if %ERRORLEVEL% EQU -2 GOTO CompilerError
@echo on

rem bin\elc src50\xforms\xforms.prj
@echo off 
if %ERRORLEVEL% EQU -2 GOTO CompilerError
@echo on

bin\elc src50\sqlite\sqlite.prj
@echo off 
if %ERRORLEVEL% EQU -2 GOTO CompilerError
@echo on

rem bin\elc src50\cellular\cellular.prj
@echo off 
if %ERRORLEVEL% EQU -2 GOTO CompilerError
@echo on

rem bin\elc src50\algorithms\algorithms.prj
@echo off 
if %ERRORLEVEL% EQU -2 GOTO CompilerError
@echo on

rem bin\elc src50\graphics\graphics.prj

bin\elc src50\system\system_64.prj
@echo off 
if %ERRORLEVEL% EQU -2 GOTO CompilerError
@echo on

bin\elc src50\extensions\extensions_64.prj
@echo off 
if %ERRORLEVEL% EQU -2 GOTO CompilerError
@echo on

bin\elc tests\system\system_test.prj
@echo off 
if %ERRORLEVEL% EQU -2 GOTO CompilerError
@echo on

tests\system\system_test.exe
@echo off 
if %ERRORLEVEL% EQU -1 GOTO TestError
@echo on

bin\elc tests\system\system_test64.prj
@echo off 
if %ERRORLEVEL% EQU -2 GOTO CompilerError
@echo on

tests\system\system_test64.exe
@echo off 
if %ERRORLEVEL% EQU -1 GOTO TestError
@echo on

@echo off 
echo === Done ===
@echo on

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

:TestError
echo System tests fail %ERRORLEVEL%
@echo off
goto:eof
@echo on
