bin\sg dat\sg\syntax.txt
move dat\sg\syntax.dat bin

bin\og dat\og\rules.txt
move dat\og\rules.dat bin

bin\asm2binx src30\asm\core_routines.esm lib30\system
bin\asm2binx src30\asm\ext_routines.esm lib30\system
bin\asm2binx src30\asm\x32\commands.asm bin\x32
bin\asm2binx src30\asm\x32\core.asm bin\x32
bin\asm2binx src30\asm\x32\corex.asm bin\x32
bin\asm2binx src30\asm\x32\coreapi.asm bin\x32
bin\asm2binx src30\asm\x32\core_vm.asm bin\x32
bin\asm2binx src30\asm\x32\core_rt.asm bin\x32
bin\asm2binx src30\asm\x32\core_win.asm bin\x32
bin\elc -csrc30\system\system.prj
bin\elc -csrc30\extensions\extensions.prj
bin\elc -csrc30\net\net.prj
bin\elc -csrc30\forms\forms.prj
bin\elc -csrc30\sqlite\sqlite.prj
