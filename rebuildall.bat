bin\sg dat\sg\syntax.txt
move dat\sg\syntax.dat bin

bin\og dat\og\rules.txt
move dat\og\rules.dat bin

bin\asm2binx src30\asm\core_routines.esm lib30\system
bin\asm2binx src30\asm\ext_routines.esm lib30\system
bin\asm2binx src30\asm\x32\commands.asm bin\x32
bin\asm2binx src30\asm\x32\core.asm bin\x32
bin\asm2binx src30\asm\x32\coreapi.asm bin\x32
bin\asm2binx src30\asm\x32\core_vm.asm bin\x32
bin\asm2binx src30\asm\x32\core_rt.asm bin\x32
bin\asm2binx src30\asm\x32\core_win.asm bin\x32
bin\elc -csrc30\system\system.prj
bin\elc -csrc30\extensions\extensions.prj
bin\elc -csrc30\forms\forms.prj
bin\elc -csrc30\sqlite\sqlite.prj
bin\elc -cexamples\helloworld\u_helloworld.prj
bin\elc -cexamples\helloworld\helloworld.prj
bin\elc -cexamples\sum\intsum.prj
bin\elc -cexamples\sum\realsum.prj
bin\elc -cexamples\words\words.prj
bin\elc -cexamples\pi\pi.prj
bin\elc -cexamples\bsort\bsort.prj
bin\elc -cexamples\textfile\textfile.prj
bin\elc -cexamples\binary\binary.prj
bin\elc -cexamples\replace\replace.prj
bin\elc -cexamples\goods\goods.prj
bin\elc -cexamples\datetime\dtcalculator.prj
bin\elc -cexamples\textdb\textdb.prj
bin\elc -cexamples\matrix\matrix.prj
bin\elc -cexamples\trans\tanslit.prj
bin\elc -cexamples\interpreter\interpreter.prj
bin\elc -cexamples\calculator\calc.prj
bin\elc -cexamples\agenda\agenda.prj
bin\elc -cexamples\graphs\graphs.prj
bin\elc -cexamples\c_a_g\c_area_gui.prj
bin\elc -cexamples\notepad\notepad.prj
bin\elc -cexamples\sqlite\sqlite_test.prj
