ELENA Language Project V. 2.0, (C)2005-2015  By Alex Rakov
====
ELENA is a general-purpose, pure object-oriented language with late binding. It promotes more object-oriented program design, reusable and more standardized code. The package includes compiler, IDE, ELENA libraries and samples
Currently only Win32-x86 (2000/XP/Vista/7/8) platform is supported.

Features
---
- Pure polymorphic object oriented language
- "Horizontal" multiple inheritance
- Dynamic "class mutation"
- Message dispatching
- ELENA Virtual machine (in developing)
- Command line 32-bit compiler
- GUI IDE & Debugger
- Unicode support (utf-8)
- Complete source code
- Number of samples
- Simple Assembler compiler

Minimum requirements
---
 - Windows XP/7/8 x86
 - Linux I386

ELENA Language modules and programs
---
The complete source code of ELENA Standard library could be found
in the folder <app root>\src30.

The source code of ELENA samples could be found
in the folder <app root>\examples.

License
---
The compiler and executables distributed in this package fall under The Apache 
License V2.0e, for more information read the file LICENSE.TXT.

ELENA Installation / Run
---
Windows:
--------
To install just unzip all the files into a directory you want.

You may need to add a path to BIN folder to system environment (e.g. <app root>\bin). 

Otherwise you may use setup executable.

To open, compile or debug the programs and libraries use ELENA GUI IDE 
(<app root>\bin\elide.exe) or ELENA Command Line Compiler 
(<app root>\bin\elc.exe).

In ELENA IDE you may select File-Open-Open Project option and open an 
appropriate project file (*.prj). Then select Project-Compile option to 
compile the project and Project-Debug to debug it.

Linux:
------
To install just unzip all the files into a directory you want and execute 
install.script.

To compile the program types : ./bin/elc2 -c<path to .project file>
