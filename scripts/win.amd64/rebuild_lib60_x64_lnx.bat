REM NOTE : the script MUST be called from the root folder

bin\asm64-cli -linux -amd64 asm\amd64\core60.asm bin\amd64 core60_lnx.bin
@echo off 
if %ERRORLEVEL% EQU -1 GOTO Asm2BinError
@echo on

bin\asm64-cli -linux -amd64 asm\amd64\corex60.asm bin\amd64 corex60_lnx.bin
@echo off 
if %ERRORLEVEL% EQU -1 GOTO Asm2BinError
@echo on

bin\asm64-cli -bc64 src60\core\system.core_routines.esm lib60_64_lnx
@echo off 
if %ERRORLEVEL% EQU -1 GOTO Asm2BinError
@echo on

bin\elena64-cli -xtlnx64 src60\elena_api.prjcol
@echo off 
if %ERRORLEVEL% EQU -2 GOTO CompilerError
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
