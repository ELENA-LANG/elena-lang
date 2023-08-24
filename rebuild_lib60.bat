bin\sg-cli dat\sg\syntax60.txt
@echo off 
if %ERRORLEVEL% EQU -1 GOTO Asm2BinError
@echo on

move dat\sg\syntax60.dat bin

bin\og-cli dat\og\bc_rules60.txt
@echo off 
if %ERRORLEVEL% EQU -1 GOTO Asm2BinError
@echo on
move dat\og\bc_rules60.dat bin

bin\og-cli -s dat\og\bt_rules60.txt 
@echo off 
if %ERRORLEVEL% EQU -1 GOTO Asm2BinError
@echo on
move dat\og\bt_rules60.dat bin

bin\asm-cli -x86 asm\x32\core60.asm bin\x32
@echo off 
if %ERRORLEVEL% EQU -1 GOTO Asm2BinError
@echo on

bin\asm-cli -x86 asm\x32\corex60.asm bin\x32
@echo off 
if %ERRORLEVEL% EQU -1 GOTO Asm2BinError
@echo on

bin\asm-cli -x86 asm\x32\core60_win.asm bin\x32
@echo off 
if %ERRORLEVEL% EQU -1 GOTO Asm2BinError
@echo on

bin\asm-cli -amd64 asm\amd64\core60.asm bin\amd64
@echo off 
if %ERRORLEVEL% EQU -1 GOTO Asm2BinError
@echo on

bin\asm-cli -amd64 asm\amd64\core60_win.asm bin\amd64
@echo off 
if %ERRORLEVEL% EQU -1 GOTO Asm2BinError
@echo on

bin\asm-cli -bc32 src60\core\system.core_routines.esm lib60
@echo off 
if %ERRORLEVEL% EQU -1 GOTO Asm2BinError
@echo on

bin\elena-cli src60\system\system.prj
@echo off 
if %ERRORLEVEL% EQU -2 GOTO CompilerError
@echo on

bin\elena-cli src60\extensions\extensions.prj
@echo off 
if %ERRORLEVEL% EQU -2 GOTO CompilerError
@echo on

bin\elena-cli src60\cellular\cellular.prj
@echo off 
if %ERRORLEVEL% EQU -2 GOTO CompilerError
@echo on

bin\asm-cli -bc64 src60\core\system.core_routines.esm lib60_64
@echo off 
if %ERRORLEVEL% EQU -1 GOTO Asm2BinError
@echo on

bin\elena64-cli src60\system\system.prj
@echo off 
if %ERRORLEVEL% EQU -2 GOTO CompilerError
@echo on

bin\elena64-cli src60\extensions\extensions.prj
@echo off 
if %ERRORLEVEL% EQU -2 GOTO CompilerError
@echo on

bin\ldoc system doc\api
bin\ldoc system'routines doc\api
bin\ldoc extensions doc\api
bin\ldoc extensions'routines doc\api
bin\ldoc extensions'scripting doc\api

bin\elena-cli tests60\system_tests\system_tests.prj
@echo off 
if %ERRORLEVEL% EQU -2 GOTO CompilerError
@echo on

bin\elena64-cli tests60\system_tests\system_tests.prj
@echo off 
if %ERRORLEVEL% EQU -2 GOTO CompilerError
@echo on

echo system api test for x86
tests60\system_tests\system_tests.exe
@echo off 
if %ERRORLEVEL% NEQ 0 GOTO TestError
@echo on

echo system api test for amd64
tests60\system_tests\system_tests64.exe
@echo off 
if %ERRORLEVEL% NEQ 0 GOTO TestError
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
