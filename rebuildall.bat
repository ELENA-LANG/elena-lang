bin\sg dat\sg\syntax.txt
move dat\sg\syntax.dat bin

bin\og dat\og\rules.txt
move dat\og\rules.dat bin

bin\asm2binx asm\core_routines.esm lib30\system
bin\asm2binx asm\x32\commands.asm bin\x32
bin\asm2binx asm\x32\core.asm bin\x32
bin\asm2binx asm\x32\corex.asm bin\x32
bin\asm2binx asm\x32\coreapi.asm bin\x32
bin\asm2binx asm\x32\core_vm.asm bin\x32
bin\asm2binx asm\x32\core_win.asm bin\x32
bin\elc -csrc30\system\system.prj
bin\elc -csrc30\extensions\extensions.prj
rem bin\elc -csrc30\net\net.prj
rem bin\elc -csrc30\forms\forms.prj
rem bin\elc -csrc30\sqlite\sqlite.prj
bin\elc -cexamples\console\helloworld\u_helloworld.prj
rem bin\elc -cexamples\console\helloworld\helloworld.prj
bin\elc -cexamples\console\sum\intsum.prj
rem bin\elc -cexamples\console\sum\realsum.prj
rem bin\elc -cexamples\console\words\words.prj
rem bin\elc -cexamples\console\pi\pi.prj
rem bin\elc -cexamples\console\bsort\bsort.prj
rem bin\elc -cexamples\files\textfile\textfile.prj
rem bin\elc -cexamples\console\binary\binary.prj
rem bin\elc -cexamples\console\replace\replace.prj
rem bin\elc -cexamples\console\goods\goods.prj
rem bin\elc -cexamples\console\datetime\dtcalculator.prj
rem bin\elc -cexamples\console\random\random.prj
rem bin\elc -cexamples\files\textdb\textdb.prj
rem bin\elc -cexamples\console\matrix\matrix.prj
rem bin\elc -cexamples\console\trans\tanslit.prj
rem bin\elc -cexamples\interpreter\interpreter.prj
rem bin\elc -cexamples\script\calculator\calc.prj
rem bin\elc -cexamples\gui\agenda\agenda.prj
rem bin\elc -cexamples\gui\graphs\graphs.prj
rem bin\elc -cexamples\gui\c_a_g\c_area_gui.prj
rem bin\elc -cexamples\gui\notepad\notepad.prj
rem bin\elc -cexamples\db\sqlite\sqlite_test.prj
rem bin\elc -cexamples\console\pi2\pi2.prj
rem bin\elc -cexamples\net\chat\chatserver.prj
rem bin\elc -cexamples\net\chat\chat.prj
