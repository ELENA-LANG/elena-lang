REM NOTE : the script MUST be called from the root folder

@echo off

ECHO =========== Copying ELENA files ==================

md build\x64
md build\x64\bin
md build\x64\bin\templates
md build\x64\bin\scripts
md build\x64\bin\amd64
md build\x64\doc
md build\x64\examples60 
md build\x64\src60
md build\x64\lib60_64

copy bin\asm64-cli.exe build\x64\bin
copy bin\elena64-cli.exe build\x64\bin
copy bin\elena64-ide.exe build\x64\bin
copy bin\sg64-cli.exe build\x64\bin
copy bin\og64-cli.exe build\x64\bin
copy bin\ecv64-cli.exe build\x64\bin
copy bin\elt64-cli.exe build\x64\bin
copy bin\elenart60_64.dll build\x64\bin
copy bin\elenasm60_64.dll build\x64\bin
copy bin\elenavm60_64.dll build\x64\bin
copy bin\winstub.ex_ build\x64\bin
copy bin\elc60.cfg build\x64\bin
copy bin\elenart60.cfg build\x64\bin
copy bin\elenavm60.cfg build\x64\bin

copy bin\*.dat build\x64\bin\
copy bin\amd64\*.bin build\x64\bin\amd64\

copy bin\templates\*.cfg build\x64\bin\templates\
copy bin\scripts\*.es build\x64\bin\scripts\
copy bin\scripts\*.elt build\x64\bin\scripts\

copy doc\license build\x64\doc\
copy doc\contributors build\x64\doc\
copy readme.md build\x64\
copy CHANGELOG.md build\x64\
copy VERSION build\x64\

xcopy src60\*.l build\x64\src60\ /s
xcopy src60\*.prj build\x64\src60\ /s

copy src60\elena_api.prjcol build\x64\src60\

xcopy lib60_64\*.nl build\x64\lib60_64\ /s
xcopy lib60_64\*.dnl build\x64\lib60_64\ /s

xcopy examples60\*.l build\x64\examples60\ /s
xcopy examples60\*.prj build\x64\examples60\ /s
xcopy examples60\*.txt build\x64\examples60\ /s
xcopy examples60\*.bmp build\x64\examples60\ /s
xcopy examples60\*.es build\x64\examples60\ /s
xcopy examples60\*.js build\x64\examples60\ /s
xcopy examples60\*.ls build\x64\examples60\ /s
xcopy examples60\*.xs build\x64\examples60\ /s

goto:eof
