bin\sg dat\sg\syntax.txt
move dat\sg\syntax.dat bin

bin\og dat\og\rules.txt
move dat\og\rules.dat bin

bin\asm2binx src30\core\core_routines.esm lib30\system
bin\asm2binx asm\x32\core.asm bin\x32
bin\asm2binx asm\x32\corex.asm bin\x32
bin\asm2binx asm\x32\coreapi.asm bin\x32
bin\asm2binx asm\x32\core_vm.asm bin\x32
bin\asm2binx asm\x32\core_win.asm bin\x32
bin\elc -csrc30\system\system.prj
rem bin\elc -csrc30\extensions\extensions.prj
rem bin\elc -csrc30\net\net.prj
rem bin\elc -csrc30\forms\forms.prj
rem bin\elc -csrc30\sqlite\sqlite.prj


rem bin\asm2binx src40\asm\core_routines.esm lib40\system
rem bin\elc -csrc40\system\system40.prj
