bin\elc examples\console\helloworld\helloworld.prj
@echo off 
if %ERRORLEVEL% EQU -2 GOTO CompilerError
@echo on

:CompilerError
echo ELC returns error %ERRORLEVEL%
@echo off
goto:eof
@echo on
