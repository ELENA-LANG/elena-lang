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
    
[![MSBuild](https://github.com/ELENA-LANG/elena-lang/actions/workflows/msbuild.yml/badge.svg?branch=master)](https://github.com/ELENA-LANG/elena-lang/actions/workflows/msbuild.yml)
[![Sponsor](https://img.shields.io/badge/patreon-donate-green.svg)](https://www.patreon.com/elena_lang)
[![Sponsor](https://img.shields.io/static/v1?label=Sponsor&message=%E2%9D%A4&logo=GitHub&link=https://github.com/sponsors/arakov)](https://github.com/sponsors/arakov)

</div>

<div align="center">
    [![ko-fi](https://ko-fi.com/img/githubbutton_sm.svg)](https://ko-fi.com/W7W01B4CV8)
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
- **Linux** : x86 (32-bit) / x86-64 (64-bit)  / ppc64le / arm64 (a64)

## Source Download and Compilation

To acquire the source code clone the git repository:

    git clone https://github.com/ELENA-LANG/elena-lang.git

### Windows:

The compiler code is implemented in C++ and does not require external dependencies. You just need Visual Studio 2019.

You have to add a path to _BIN_ folder to the system environment *PATH* or copy elenavm.dll and elenart.dll to _Windows\System32_ folder.

To build the compiler and API under VS2019 / VS2022 you have to go to the root folder and type:

    recompile60.bat 

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
    elenasrc3\elc       source for the compiler
    elenasrc3\elenart   source for the run-time shared library
    elenasrc3\elenasm   source for the script engine
    elenasrc3\elenavm   source for the virtual machine
    elenasrc3\gui       source for IDE
    elenasrc3\tools     source for ELENA utilities
    examples60          ELENA examples
    src60               source for ELENA libraries

## Community
We want your contributions and suggestions! One of the easiest ways to contribute is to participate in Github discussions or on Discord.

Please take a look at our [contributor page](https://github.com/ELENA-LANG/elena-lang/wiki/Getting-Started-with-the-project)

### 1. Bugs, questions, suggestions?

If you've noticed a bug or have a question go ahead and [make one](https://github.com/ELENA-LANG/elena-lang/issues/new/choose)!

[Join](https://github.com/ELENA-LANG/elena-lang/issues?q=is%3Aissue+is%3Aopen+label%3A%22Design+Idea%22) design discussions or [take part](https://github.com/ELENA-LANG/elena-lang/issues?q=is%3Aissue+is%3Aopen+label%3ADiscussion) in general talks.

### 2. Implement "up for grab" issues

[Good first issue](https://github.com/ELENA-LANG/elena-lang/labels/good%20first%20issue) is a good starting point for a first-time contributors.

### 3. Rosetta code

You may try to implement some of [Rosetta code tasks](http://rosettacode.org/wiki/Reports:Tasks_not_implemented_in_Elena) 
which are not yet implemented

## Contact Us

Reach out with any questions you may have and we'll make sure to answer them as soon as possible!

| Platform  | Link        |
|:----------|:------------|
| 💬 Instant Message Chat | [![Discord Banner](https://discordapp.com/api/guilds/1023392280087908352/widget.png?style=banner2)](https://discord.gg/pMCjunWSxH)
| 📧 E-mail | elenaprolang@gmail.com
| 🐤 Twitter | [@elena_language](https://twitter.com/elena_language)

## Resources
- **Nightly builds:** <https://ci.appveyor.com/project/arakov/elena-lang/build/artifacts>
- **ELENA Documentation** <https://github.com/ELENA-LANG/elena-lang/wiki/ELENA-Programming-Manual>
- **ELENA API 6.0** <https://elena-lang.github.io/api/index.html>
- **Git clone URL:** <git://github.com/ELENA-LANG/elena-lang.git>
- **Tutorials:** <https://github.com/ELENA-LANG/tutorials>
- **ELENA reddit:** <https://www.reddit.com/r/elena_lang/>
- **Source code:** <https://github.com/ELENA-LANG/elena-lang>
- **Twitter:** <https://twitter.com/elena_language>
- **Rosetta code:** <http://rosettacode.org/wiki/Category:Elena>

## License

The compiler and executables distributed in this package fall under MIT License, 
for more information read the file LICENSE.
