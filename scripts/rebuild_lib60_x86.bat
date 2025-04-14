REM NOTE : the script MUST be called from the root folder

bin\asm-cli -windows -x86 asm\x32\core60.asm bin\x32 core60_win.bin
@echo off 
if %ERRORLEVEL% EQU -1 GOTO Asm2BinError
@echo on

bin\asm-cli -windows -x86 asm\x32\corex60.asm bin\x32 corex60_win.bin
@echo off 
if %ERRORLEVEL% EQU -1 GOTO Asm2BinError
@echo on

bin\asm-cli -x86 asm\x32\core60_win_client.asm bin\x32
@echo off 
if %ERRORLEVEL% EQU -1 GOTO Asm2BinError
@echo on

bin\asm-cli -bc32 src60\core\system.core_routines.esm lib60
@echo off 
if %ERRORLEVEL% EQU -1 GOTO Asm2BinError
@echo on

bin\asm-cli -bc32 src60\core\system.win_core_routines.esm lib60
@echo off 
if %ERRORLEVEL% EQU -1 GOTO Asm2BinError
@echo on

bin\elena-cli src60\elena_api.prjcol
@echo off 
if %ERRORLEVEL% EQU -2 GOTO CompilerError
@echo on

bin\ldoc system doc\api
bin\ldoc system'collections'threadsafe doc\api
bin\ldoc system'routines doc\api
bin\ldoc system'runtime doc\api
bin\ldoc system'threading doc\api
bin\ldoc system'dynamic doc\api
bin\ldoc system'drawing doc\api
bin\ldoc system'winforms doc\api
bin\ldoc system'net doc\api
bin\ldoc extensions doc\api
bin\ldoc extensions'routines doc\api
bin\ldoc extensions'scripting doc\api
bin\ldoc extensions'dynamic doc\api
bin\ldoc extensions'io doc\api
bin\ldoc cellular doc\api
bin\ldoc algorithms doc\api
bin\ldoc sqlite doc\api
bin\ldoc forms doc\api
bin\ldoc ltests doc\api
bin\ldoc net doc\api

bin\elena-cli tests60\system_tests\system_tests.prj
@echo off 
if %ERRORLEVEL% EQU -2 GOTO CompilerError
@echo on

echo system api test for x86
copy bin\elenart60.dll tests60\system_tests\
copy bin\elenasm60.dll tests60\system_tests\

tests60\system_tests\system_tests.exe
@echo off 
if %ERRORLEVEL% NEQ 0 GOTO TestError
@echo on

REM bin\elena-cli tests60\script_tests\script_tests.prj
REM @echo off 
REM if %ERRORLEVEL% EQU -2 GOTO CompilerError
REM @echo on

REM echo system api test for x86
REM copy bin\elenart60.dll tests60\script_tests\
REM copy bin\elenasm60.dll tests60\script_tests\

REM tests60\script_tests\script_tests.exe
REM @echo off 
REM if %ERRORLEVEL% NEQ 0 GOTO TestError
REM @echo on

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