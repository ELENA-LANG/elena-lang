@echo off

ECHO =========== Compiling ELENA files ==================

md %~dp0\bin

copy %~dp0\..\..\bin\asm2binx.exe %~dp0\bin
copy %~dp0\..\..\bin\elc.exe %~dp0\bin
copy %~dp0\..\..\bin\elide.exe %~dp0\bin
copy %~dp0\..\..\bin\sg.exe %~dp0\bin
copy %~dp0\..\..\bin\elt.exe %~dp0\bin
copy %~dp0\..\..\bin\ecv.exe %~dp0\bin
copy %~dp0\..\..\bin\og.exe %~dp0\bin

%~dp0\..\..\bin\sg %~dp0\..\..\dat\sg\syntax.txt
move %~dp0..\..\dat\sg\syntax.dat %~dp0\bin\

goto:eof
::ERRORS
::---------------------
:CompilerError
echo The MSBuild returns error %ERRORLEVEL%
goto:eof
