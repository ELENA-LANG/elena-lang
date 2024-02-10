cd calc
..\..\..\bin\elena-cli calc.prj
@echo off 
if %ERRORLEVEL% EQU -2 GOTO CompilerError2
@echo on
cd ..

..\..\bin\elena-cli -pinterpreter interpreter.l 
@echo off 
if %ERRORLEVEL% EQU -2 GOTO CompilerError
@echo on

cd js
..\..\..\bin\elena-cli jsinterpreter.prj 
@echo off 
if %ERRORLEVEL% EQU -2 GOTO CompilerError2
@echo on
cd ..

cd ls
..\..\..\bin\elena-cli lsinterpreter.prj
@echo off 
if %ERRORLEVEL% EQU -2 GOTO CompilerError2
@echo on
cd ..

@echo off 
goto:eof
@echo on

:CompilerError
echo ELC returns error %ERRORLEVEL%
@echo off
goto:eof
@echo on

:CompilerError2
cd ..
echo ELC returns error %ERRORLEVEL%
@echo off
goto:eof
@echo on
