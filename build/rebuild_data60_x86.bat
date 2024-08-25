REM NOTE : the script MUST be called from the root folder

.\bin\sg-cli dat\sg\syntax60.txt
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