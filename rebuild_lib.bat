bin\sg dat\sg\syntax.txt
move dat\sg\syntax.dat bin

bin\og dat\og\rules.txt
move dat\og\rules.dat bin

bin\asm2binx src31\core\core_routines.esm lib31\system
bin\asm2binx asm\x32\core.asm bin\x32
bin\asm2binx asm\x32\corex.asm bin\x32                
bin\asm2binx asm\x32\coreapi.asm bin\x32
bin\asm2binx asm\x32\core_vm.asm bin\x32
bin\asm2binx asm\x32\core_win.asm bin\x32

bin\asm2binx -amd64 asm\amd64\core.asm bin\amd64
bin\asm2binx -amd64 asm\amd64\core_win.asm bin\amd64
bin\asm2binx -amd64 asm\amd64\coreapi.asm bin\amd64

bin\elc src31\system\system.prj
bin\elc src31\extensions\extensions.prj
rem bin\elc src31\net\net.prj
rem bin\elc src31\forms\forms.prj
bin\elc src31\sqlite\sqlite.prj
rem bin\elc src31\cellular\cellular.prj

rem bin\elc src31\system\system_64.prj
