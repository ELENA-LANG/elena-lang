[![Build status](https://ci.appveyor.com/api/projects/status/qc5dvs5lueubivuo?svg=true)](https://ci.appveyor.com/project/arakov/elena-lang)

# ELENA Programming Language

ELENA is a general-purpose, object-oriented language with late binding. It features message dispatching, mixins and a script engine.
This is the GitHub repository of ELENA source code including compiler, IDE, ELENA libraries and samples.

## Resources
- **Wiki:** <https://github.com/ELENA-LANG/elena-lang/wiki/>
- **ELENA reddit:** <https://www.reddit.com/r/elena_lang/>
- **Source code:** <https://github.com/ELENA-LANG/elena-lang>
- **Git clone URL:** <git://github.com/ELENA-LANG/elena-lang.git>
- **Twitter:** <https://twitter.com/elena_language>
- **Rosetta code:** <http://rosettacode.org/wiki/Category:Elena>
- **Homepage:** <http://elenalang.sourceforge.net/>
- **Blog:** <http://elenalang.blogspot.com/>
- **Binaries mirror:** <https://sourceforge.net/projects/elenalang/files/>
- **Slack** <https://elenalang.slack.com/>

## Currently Supported Platforms

- **Linux**
- **Windows**

For time being only x86 (32-bit) architecture is supported. Work on x86/64 (64-bit) goes on.

## Source Download and Compilation

To acquire the source code clone the git repository:

    git clone git://github.com//ELENA-LANG/elena-lang.git

### Windows:

The compiler code is implemented in C++ and does not require external dependencies. You just need Visual Studio 2015 or 2017.

You have to add a path to _BIN_ folder to the system environment *PATH* or copy elenavm.dll and elenart.dll to _Windows\System32_ folder.

To build the compiler under VS2015 you have to go to the root folder and type:

    recompile.bat

If you use VS2017 type the following:

    recompile15.bat

To build the ELENA libraries type the following command:

    rebuild_lib.bat

## Contribute!

ELENA Project looks for programmers interested in learning the language and providing feedback.

### 1. Bugs, questions, suggestions?

If you've noticed a bug or have a question go ahead and [make one](https://github.com/ELENA-LANG/elena-lang/issues/new)!

### 2. Implement "up for grab" issues

You may implement one of the open [issues](https://github.com/ELENA-LANG/elena-lang/labels/up%20for%20grabs).

### 3. Rosetta code

You may try to implementing some of [Rosetta code tasks](http://rosettacode.org/wiki/Category:Programming_Tasks) 
which are not yet implemented

## Source Code Organization

### Windows:

The ELENA source code is organized as follows:

    bin                 binaries and shared libraries
    bin\scripts         scripts used by the script engine and VM console
    bin\templates       ELENA project templates
    asm                 source for core routines implemented in assembly
    dat\sg              language grammar file            
    dat\og              language optimization rules
    doc                 some documentations
    elenasrc2\elc       source for the compiler
    elenasrc2\elenart   source for the run-time shared library
    elenasrc2\elenasm   source for the script engine
    elenasrc2\elenavm   source for the virtual machine
    elenasrc2\gui       source for IDE
    elenasrc2\tools     source for ELENA utilities
    examples            ELENA examples
    src31               source for ELENA libraries

## License

The compiler and executables distributed in this package fall under MIT License, 
for more information read the file LICENSE.

## Questions?

Feel free to ask any question :  elenaprolang@gmail.com.
