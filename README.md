<div align="center">
<p>
    <img src="https://user-images.githubusercontent.com/5677187/84074668-31633400-a9d3-11ea-9fd6-9282ab80b537.jpg">
</p>
<h1>ELENA Programming Language</h1>

[elena-lang.github.io](https://elena-lang.github.io/) |
[Docs](https://github.com/ELENA-LANG/elena-lang/wiki/ELENA-Programming-Manual) |
[Changelog](https://github.com/ELENA-LANG/elena-lang/blob/master/CHANGELOG.md) |
[Contributing](https://github.com/ELENA-LANG/elena-lang/blob/master/CONTRIBUTING.md)

</div>

<div align="center">
    
[![Build status](https://ci.appveyor.com/api/projects/status/qc5dvs5lueubivuo?svg=true)](https://ci.appveyor.com/project/arakov/elena-lang)
[![Sponsor](https://img.shields.io/badge/patreon-donate-green.svg)](https://www.patreon.com/elena_lang)
[![Sponsor](https://img.shields.io/static/v1?label=Sponsor&message=%E2%9D%A4&logo=GitHub&link=https://github.com/sponsors/arakov)](https://github.com/sponsors/arakov)

</div>

ELENA is a general-purpose language with late binding. It is multi-paradigm, combining features of functional and object-oriented programming. It supports both strong and weak types, run-time conversions, boxing and unboxing primitive types, direct usage of external libraries. A rich set of tools is provided to deal with message dispatching : multi-methods, message qualifying, generic message handlers. Multiple-inheritance can be simulated using mixins and type interfaces. The built-in script engine allows incorporating custom-defined scripts into your applications. Both stand-alone applications and Virtual machine clients are supported.

### Features

  - Free and open-source (MIT licensed)
  - Complete source code
  - Unicode support (utf-8)
  - GUI IDE & Debugger
  - Optional types
  - Multiple dispatching / multi-methods
  - Returning Multiple Values
  - Support of variadic methods
  - Support of yieldable methods
  - Closures
  - Mixins
  - Type interfaces / conversions
  - Class / code templates
  - Script Engine

## Currently Supported Platforms

- **Windows** : x86 (32-bit) / x86-64 (64-bit) 
- **Linux** : x86 (32-bit)

## Source Download and Compilation

To acquire the source code clone the git repository:

    git clone git://github.com/ELENA-LANG/elena-lang.git

### Windows:

The compiler code is implemented in C++ and does not require external dependencies. You just need Visual Studio 2019.

You have to add a path to _BIN_ folder to the system environment *PATH* or copy elenavm.dll and elenart.dll to _Windows\System32_ folder.

To build the compiler under VS2019 you have to go to the root folder and type:

    recompile19.bat

To build the ELENA libraries type the following command:

    rebuild_lib.bat
    
To build ELENA samples type the following command:

    rebuild_examples.bat 
    
To build ELENA Rosetta-code samples type the following command:

    examples\rosetta\build.bat 
    
To run unit-tests type the following command:

    lib_tests.bat     

## Contribute!

ELENA Project looks for programmers interested in learning the language and providing feedback.

### 1. Bugs, questions, suggestions?

If you've noticed a bug or have a question go ahead and [make one](https://github.com/ELENA-LANG/elena-lang/issues/new/choose)!

[Join](https://github.com/ELENA-LANG/elena-lang/issues?q=is%3Aissue+is%3Aopen+label%3A%22Design+Idea%22) design discussions or [take part](https://github.com/ELENA-LANG/elena-lang/issues?q=is%3Aissue+is%3Aopen+label%3ADiscussion) in general talks.

### 2. Implement "up for grab" issues

[Good first issue](https://github.com/ELENA-LANG/elena-lang/labels/good%20first%20issue) is a good starting point for a first-time contributors.

### 3. Rosetta code

You may try to implement some of [Rosetta code tasks](http://rosettacode.org/wiki/Reports:Tasks_not_implemented_in_Elena) 
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
    src50               source for ELENA libraries

## License

The compiler and executables distributed in this package fall under MIT License, 
for more information read the file LICENSE.

## Questions?

Feel free to ask any question:  elenaprolang@gmail.com.

## Resources
- **Nightly builds:** <https://ci.appveyor.com/project/arakov/elena-lang/build/artifacts>
- **ELENA Documentation** <https://github.com/ELENA-LANG/elena-lang/wiki/ELENA-Programming-Manual>
- **ELENA API 5.0** <https://elena-lang.github.io/api/index.html>
- **Git clone URL:** <git://github.com/ELENA-LANG/elena-lang.git>
- **Tutorials:** <https://github.com/ELENA-LANG/tutorials>
- **ELENA reddit:** <https://www.reddit.com/r/elena_lang/>
- **Source code:** <https://github.com/ELENA-LANG/elena-lang>
- **Twitter:** <https://twitter.com/elena_language>
- **Rosetta code:** <http://rosettacode.org/wiki/Category:Elena>
