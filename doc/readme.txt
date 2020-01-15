                      ELENA Language Project V. 5.0
                        (C)2005-2020  By Alex Rakov
    
The project includes ELENA documentation, standard library source code, 
samples, command-line compiler, IDE.

Currently Win32-x86 (Windows 7/8/10) / Linux-I386 (alpha) platforms are supported.

For more information see at http://elenalang.sourceforge.net/

****************************************************************************
* Features
****************************************************************************

- Pure polymorphic object oriented language
- Dynamic "class mutation"
- Message dispatching
- ELENA Virtual machine
- Command line 32-bit compiler
- GUI IDE & Debugger
- Unicode support (utf-8)
- Complete source code
- Number of samples
- Getting started tutorial
- Simple Assembler compiler
- Script Engine / Interpreter
- ELENA Virtual Machine Terminal

****************************************************************************
* OS requirements
****************************************************************************

 - Windows 7 / 8 / 10, x86
 - Linux I386

****************************************************************************
* License
****************************************************************************

The compiler and executables distributed in this package fall under MIT License, 
for more information read the file LICENSE.

****************************************************************************
* ELENA Installation / Run
****************************************************************************

Windows:
--------
To install just unzip all the files into a directory you want.

You may need to add a path to BIN folder to system environment (e.g. <app root>\bin). 

Otherwise you may use setup executable.

The language requires Visual C++ Redistributable for VS 2017

To open, compile or debug the programs and libraries use ELENA GUI IDE 
(<app root>\bin\elide.exe) or ELENA Command Line Compiler 
(<app root>\bin\elc.exe).

In ELENA IDE you may select File-Open-Open Project option and open an 
appropriate project file (*.prj). Then select Project-Compile option to 
compile the project and Project-Debug to debug it.

Linux:
------
To install just unzip all the files into a directory you want and execute 
rebuild_lib.script under the superuser. Alternatively install debian package

 dpkg -i /path/to/deb/file
 sudo apt-get install -f

Examples are located in /usr/elena-lang

To compile the program types : elena-lc <path to .project file>

****************************************************************************
* Documentation
****************************************************************************

The documentation is available on http://github.com/ELENA-LANG/elena-lang/wiki

****************************************************************************
* Suggestions, Help, Bug reporting  ...
****************************************************************************

Suggestions, Help ...
---------------------
http://github.com/ELENA-LANG/elena-lang/issues
    - tickets / issues / questions / bug reports

https://www.reddit.com/r/elena_lang/
    - reddit

elenaprolang@gmail.com
    - questions

****************************************************************************
* ELENA Language modules and programs
****************************************************************************
                                                
The complete source code of ELENA Standard library could be found
in the folder <app root>\src40.

The source code of ELENA samples could be found
in the folder <app root>\examples.

****************************************************************************
* ELENA Project Source code
****************************************************************************

The project source code is compiled with Visual Studio 2017

The project files could be found in <app root>\elenasrc2 folder.
