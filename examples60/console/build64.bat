..\..\bin\elena64-cli helloworld\helloworld.prj
@echo off 
if %ERRORLEVEL% EQU -2 GOTO CompilerError
@echo on

..\..\bin\elena64-cli sum\intsum.prj
@echo off 
if %ERRORLEVEL% EQU -2 GOTO CompilerError
@echo on

..\..\bin\elena64-cli sum\realsum.prj
@echo off 
if %ERRORLEVEL% EQU -2 GOTO CompilerError
@echo on

..\..\bin\elena64-cli -pwords words.l
@echo off 
if %ERRORLEVEL% EQU -2 GOTO CompilerError
@echo on

..\..\bin\elena64-cli -pbinary binary.l
@echo off 
if %ERRORLEVEL% EQU -2 GOTO CompilerError
@echo on

..\..\bin\elena64-cli -pbsort bsort.l
@echo off 
if %ERRORLEVEL% EQU -2 GOTO CompilerError
@echo on

@echo off 
goto:eof
@echo on

:CompilerError
echo ELC returns error %ERRORLEVEL%
@echo off
goto:eof
@echo on
