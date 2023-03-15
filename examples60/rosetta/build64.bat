..\..\bin\elena64-cli -packermann ackermann.l
@echo off 
if %ERRORLEVEL% EQU -2 GOTO CompilerError
@echo on

..\..\bin\elena64-cli -paccumulator accumulator.l
@echo off 
if %ERRORLEVEL% EQU -2 GOTO CompilerError
@echo on

..\..\bin\elena64-cli -paddfield addfield.l
@echo off 
if %ERRORLEVEL% EQU -2 GOTO CompilerError
@echo on

..\..\bin\elena64-cli -panonymrec anonymrec.l
@echo off 
if %ERRORLEVEL% EQU -2 GOTO CompilerError
@echo on

..\..\bin\elena64-cli -paplusb aplusb.l
@echo off 
if %ERRORLEVEL% EQU -2 GOTO CompilerError
@echo on

..\..\bin\elena64-cli -papplycallback applycallback.l
@echo off 
if %ERRORLEVEL% EQU -2 GOTO CompilerError
@echo on

..\..\bin\elena64-cli -parithmeticint arithmeticint.l
@echo off 
if %ERRORLEVEL% EQU -2 GOTO CompilerError
@echo on

..\..\bin\elena64-cli -parithmeval arithmeval.l
@echo off 
if %ERRORLEVEL% EQU -2 GOTO CompilerError
@echo on

..\..\bin\elena64-cli -parithmmean arithmmean.l
@echo off 
if %ERRORLEVEL% EQU -2 GOTO CompilerError
@echo on

..\..\bin\elena64-cli -parrayconcat arrayconcat.l
@echo off 
if %ERRORLEVEL% EQU -2 GOTO CompilerError
@echo on

..\..\bin\elena64-cli -parraymode arraymode.l
@echo off 
if %ERRORLEVEL% EQU -2 GOTO CompilerError
@echo on

..\..\bin\elena64-cli -passociativearrays associativearrays.l
@echo off 
if %ERRORLEVEL% EQU -2 GOTO CompilerError
@echo on

..\..\bin\elena64-cli -pbestshuffle bestshuffle.l
@echo off 
if %ERRORLEVEL% EQU -2 GOTO CompilerError
@echo on

..\..\bin\elena64-cli -pbinary binary.l
@echo off 
if %ERRORLEVEL% EQU -2 GOTO CompilerError
@echo on

..\..\bin\elena64-cli -pbitwise bitwise.l
@echo off 
if %ERRORLEVEL% EQU -2 GOTO CompilerError
@echo on

..\..\bin\elena64-cli -pbrackets brackets.l
@echo off 
if %ERRORLEVEL% EQU -2 GOTO CompilerError
@echo on

..\..\bin\elena64-cli -pbullscows bullscows.l
@echo off 
if %ERRORLEVEL% EQU -2 GOTO CompilerError
@echo on

..\..\bin\elena64-cli -pcaesar caesar.l
@echo off 
if %ERRORLEVEL% EQU -2 GOTO CompilerError
@echo on

..\..\bin\elena64-cli -pcharmatch charmatch.l
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
