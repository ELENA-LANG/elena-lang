tests\system\system_test
@echo off 
if %ERRORLEVEL% EQU -1 GOTO TestError
@echo on

@echo off 
goto:eof
@echo on

:TestError
echo Test fails!
@echo off
goto:eof
@echo on
