                      ELENA Language Project V. 3.1.1
                        (C)2005-2017  By Alex Rakov
    
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

 - Windows XP(with some limitations) / 7 / 8 / 10, x86
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

The language requires Visual C++ Redistributable for VS 2015

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

Visit the project web site for the latest info on the project: 
http://elenalang.netu.cf/

The documentation is available as HTML pages, and text. 
These are all available on http://github.com/ELENA-LANG/elena-lang/wiki
(see Docs page) and in <app root>\doc

There you also can find Getting Started tutorial

****************************************************************************
* Suggestions, Help, Bug reporting  ...
****************************************************************************

Suggestions, Help ...
---------------------
http://github.com/ELENA-LANG/elena-lang/issues
    - tickets / issues / questions

http://elenalang.blogspot.com/
    - tutorials

elenaprolang@gmail.com
    - questions / bug reports

****************************************************************************
* ELENA Language modules and programs
****************************************************************************
                                                
The complete source code of ELENA Standard library could be found
in the folder <app root>\src30.

The source code of ELENA samples could be found
in the folder <app root>\examples.

****************************************************************************
* ELENA Language API & Documentation
****************************************************************************

ELENA API Documentation could be found in the folder <app root>\doc\api.
Number of other documents (todo list, known bugs, road map) are located in
<app root>\doc. For any suggestion, comments or correction please contact
the author via twitter - https://twitter.com/elena_language 
or github - https://github.com/ELENA-LANG/elena-lang/issues

****************************************************************************
* ELENA Project Source code
****************************************************************************

The project source code is compiled with CodeBlocks and Mingw32

The project files could be found in <app root>\elenasrc2 folder.

Visual studio express projects are available as well.
