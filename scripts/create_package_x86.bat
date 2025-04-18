REM NOTE : the script MUST be called from the root folder

@echo off

ECHO =========== Copying ELENA files ==================

md build\x86
md build\x86\bin
md build\x86\bin\templates
md build\x86\bin\scripts
md build\x86\bin\x32
md build\x86\doc
md build\x86\examples60 
md build\x86\src60
md build\x86\lib60

dat\api2html\api2html.bat

copy bin\asm-cli.exe build\x86\bin
copy bin\elena-cli.exe build\x86\bin
copy bin\elena-ide.exe build\x86\bin
copy bin\sg-cli.exe build\x86\bin
copy bin\og-cli.exe bindx86\bin
copy bin\ecv-cli.exe build\x86\bin
copy bin\elt-cli.exe build\x86\bin
copy bin\elenart60.dll build\x86\bin
copy bin\elenasm60.dll build\x86\bin
copy bin\elenavm60.dll build\x86\bin
copy bin\winstub.ex_ build\x86\bin
copy bin\elc60.cfg build\x86\bin
copy bin\elenart60.cfg build\x86\bin
copy bin\elenavm60.cfg build\x86\bin
copy bin\*.es build\x86\bin

copy bin\*.dat build\x86\bin\
copy bin\x32\*.bin build\x86\bin\x32\

copy bin\templates\*.cfg build\x86\bin\templates\
copy bin\scripts\*.es build\x86\bin\scripts\

copy doc\license build\x86\doc\
copy doc\contributors build\x86\doc\
copy readme.md build\x86\
copy CHANGELOG.md build\x86\
copy VERSION build\x86\

xcopy src60\*.l build\x86\src60\ /s
xcopy src60\*.prj build\x86\src60\ /s

copy src60\elena_api.prjcol build\x86\src60\

xcopy lib60\*.nl build\x86\lib60\ /s
xcopy lib60\*.dnl build\x86\lib60\ /s

xcopy examples60\*.l build\x86\examples60\ /s
xcopy examples60\*.prj build\x86\examples60\ /s
xcopy examples60\*.txt build\x86\examples60\ /s
xcopy examples60\*.bmp build\x86\examples60\ /s
xcopy examples60\*.es build\x86\examples60\ /s
xcopy examples60\*.js build\x86\examples60\ /s
xcopy examples60\*.ls build\x86\examples60\ /s
xcopy examples60\*.xs build\x86\examples60\ /s

goto:eof
