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
bin\elc -cexamples\console\helloworld\u_helloworld.prj
bin\elc -cexamples\console\helloworld\helloworld.prj
bin\elc -cexamples\console\sum\intsum.prj
bin\elc -cexamples\console\sum\realsum.prj
bin\elc -cexamples\console\words\words.prj
bin\elc -cexamples\console\pi\pi.prj
bin\elc -cexamples\console\bsort\bsort.prj
bin\elc -cexamples\files\textfile\textfile.prj
bin\elc -cexamples\console\binary\binary.prj
bin\elc -cexamples\console\replace\replace.prj
bin\elc -cexamples\console\goods\goods.prj
bin\elc -cexamples\console\datetime\dtcalculator.prj
bin\elc -cexamples\files\textdb\textdb.prj
bin\elc -cexamples\console\matrix\matrix.prj
bin\elc -cexamples\console\trans\tanslit.prj
rem bin\elc -cexamples\interpreter\interpreter.prj
bin\elc -cexamples\script\calculator\calc.prj
bin\elc -cexamples\gui\agenda\agenda.prj
bin\elc -cexamples\gui\graphs\graphs.prj
bin\elc -cexamples\gui\c_a_g\c_area_gui.prj
bin\elc -cexamples\gui\notepad\notepad.prj
bin\elc -cexamples\db\sqlite\sqlite_test.prj
