REM NOTE : the script MUST be called from the root folder

bin\asm64-cli -amd64 asm\amd64\core60.asm bin\amd64
@echo off 
if %ERRORLEVEL% EQU -1 GOTO Asm2BinError
@echo on

bin\asm64-cli -amd64 asm\amd64\core60_win.asm bin\amd64
@echo off 
if %ERRORLEVEL% EQU -1 GOTO Asm2BinError
@echo on

bin\asm64-cli -amd64 asm\amd64\corex60.asm bin\amd64
@echo off 
if %ERRORLEVEL% EQU -1 GOTO Asm2BinError
@echo on

bin\asm64-cli -amd64 asm\amd64\corex60_win.asm bin\amd64
@echo off 
if %ERRORLEVEL% EQU -1 GOTO Asm2BinError
@echo on

bin\asm64-cli -windows -amd64 asm\amd64\core60_client.asm bin\amd64 core60_win_client.bin
@echo off 
if %ERRORLEVEL% EQU -1 GOTO Asm2BinError
@echo on

bin\asm64-cli -bc64 src60\core\system.core_routines.esm lib60_64
@echo off 
if %ERRORLEVEL% EQU -1 GOTO Asm2BinError
@echo on

bin\asm64-cli -bc64 src60\core\system.win_core_routines.esm lib60_64
@echo off 
if %ERRORLEVEL% EQU -1 GOTO Asm2BinError
@echo on

bin\elena64-cli src60\elena_api.prjcol
@echo off 
if %ERRORLEVEL% EQU -2 GOTO CompilerError
@echo on

bin\elena64-cli tests60\system_tests\system_tests.prj
@echo off 
if %ERRORLEVEL% EQU -2 GOTO CompilerError
@echo on

echo system api test for amd64
copy bin\elenart60_64.dll tests60\system_tests\
copy bin\elenasm60_64.dll tests60\system_tests\

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
