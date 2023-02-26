..\..\bin\elena-cli -packermann ackermann.l
@echo off 
if %ERRORLEVEL% EQU -2 GOTO CompilerError
@echo on

..\..\bin\elena-cli -paccumulator accumulator.l
@echo off 
if %ERRORLEVEL% EQU -2 GOTO CompilerError
@echo on

..\..\bin\elena-cli -paddfield addfield.l
@echo off 
if %ERRORLEVEL% EQU -2 GOTO CompilerError
@echo on

..\..\bin\elena-cli -panonymrec anonymrec.l
@echo off 
if %ERRORLEVEL% EQU -2 GOTO CompilerError
@echo on

..\..\bin\elena-cli -paplusb aplusb.l
@echo off 
if %ERRORLEVEL% EQU -2 GOTO CompilerError
@echo on

..\..\bin\elena-cli -papplycallback applycallback.l
@echo off 
if %ERRORLEVEL% EQU -2 GOTO CompilerError
@echo on

..\..\bin\elena-cli -parithmeticint arithmeticint.l
@echo off 
if %ERRORLEVEL% EQU -2 GOTO CompilerError
@echo on

..\..\bin\elena-cli -parithmeval arithmeval.l
@echo off 
if %ERRORLEVEL% EQU -2 GOTO CompilerError
@echo on

..\..\bin\elena-cli -parithmmean arithmmean.l
@echo off 
if %ERRORLEVEL% EQU -2 GOTO CompilerError
@echo on

..\..\bin\elena-cli -parrayconcat arrayconcat.l
@echo off 
if %ERRORLEVEL% EQU -2 GOTO CompilerError
@echo on

..\..\bin\elena-cli -parraymode arraymode.l
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