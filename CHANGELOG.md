## ELENA 6.4.0

- ELENA
  - [FIXED] method reference : support function singletons
  - [FIXED] direct typecasting must have priority over implicit conversion
  - [ADDED] Thread static symbols / static fields
  - [ADDED] new byte-codes : peektls / storetls
  - [FIXED] ?? operator
  - [ADDED] static constructor
  - [ADDED] internal symbols

- ELC
  - [ADDED] new option -el5 / -el6 (default) - specifying grammar compatible to ELENA 5.x or 6.x
  - [ADDED] Win x32-64 : MTA Console
	
- VM

- RT

- SM

- API
  - [ADDED] system'threading'Thread: Priority, join<int>, Current
  - [ADDED] system'runtime'Environment
  - [ADDED] system'collections'threadsafe'ConcurrentQueue<T>
  - [ADDED] #154 : system'threading'Semaphore
  - [ADDED] system'threading'BlockingQueue<T>
  - [ADDED]system'threading'ThreadPool

- SAMPLES

- Tools

- IDE
  - [FIXED] #679 : class already exist

## ELENA 6.3.0

- ELENA
  - [ADDED] key-value expression
  - [ADDED] #265 : Support enumeration list
  - [ADDED] self attribute
  - [ADDED] new operator $size
  - [ADDED] text blocks
  - [ADDED] method reference - &myMethod
  - [FIXED] single dispatcher : if an argument is nillable, it can accept nil value
  - [ADDED] String interpolation 
  - [REDUX] iterator method
  - [ADDED] #496 : private fields

- ELC
  - [FIXED] private constructor must be called directly
  - [FIXED] accessing static fields inside a structure
  - [FIXED] ppc64le : decoratorTest()
  - [FIXED] #667 : Boxing the symbol expression
  - [FIXED] only public classes can be loaded in run-time
  - [ADDED] #637 : bt optimization 4 unit test
  - [FIXED] var attribute is allowed to be in the method argument list
  - [FIXED] "__typeof self" expression inside the nested class / closure

- VM
  - [FIXED] GC_ALLOC routine for vm mode 

- RT

- SM
  - [ADDED] CF : alternative output

- API
  - [ADDED] reusing PermVectorTable after windows are closed
  - [ADDED] xforms60 script
  - [FIXED] external calls to be excluded from managed stack frames
  - [FIXED] Directory.getFiles : raising an exception if no files were found

- SAMPLES
  - [ADDED] xforms example

- Tools
  - [ADDED] #658 : new project LDBG - ELENA Debugger Adapter

- IDE

- Misc

## ELENA 6.2.0
*25.06.2024*

- ELC
  - [ADDED] optimized ternary expressions
  - [FIXED] declaring a field with a type of template based array
  - [ADDED] unit-test for declaring a field with a type of template based array
  - [FIXED] function attribute is allowed for the function declaration
  - [FIXED] template generation routines
  - [FIXED] include opcode must restore previous frame value 

- VM
  - [ADDED] #590 : creating a dynamic interface wrapper
  - [FIXED] JIT code resolving

- RT
  - [ADDED] #590 : creating a dynamic interface wrapper

- API
  - [ADDED] #89 : new functionality - MemoryStream, BitArray, SortedList, SortedArrayList
  - [ADDED] #95 : LiteralBuffer, WideBuffer
  - [ADDED] new template extensions'dynamic'Serializer<T>
  - [FIXED] String#class.copy[4] 

- SAMPLES
  - [ADDED] chat sample

## ELENA 6.0.10
*07.05.2024*

- ELENA
  - [ADDED] #646 : Calling own method directly without target
  - [ADDED] #651 : Template Expression - typecasting 
  - [ADDED] #497 : a static method must have an access to private methods

- ELC
  - [FIXED] aarch64 : xstorefir opcode
  - [FIXED] ppc64le : open opcode
  - [ADDED] new option : auto-preloaded module extension ("-xm")
  - [FIXED] reducing the total size of an executable by ignoring module extensions by default
  - [FIXED] #401 - += operator with array
  - [FIXED] #488 : Declaring auto struct variable
  - [FIXED] #522 : single dispatch variadic message
  - [FIXED] x86-64 : calling from external
  - [FIXED] #608 : single dispatch for constructors
  - [FIXED] #504 : Support an array operation with external function arguments
  - [FIXED] #589 : Supporting ?: operator inside ??
  - [ADDED] #569 : Ignoring order of declaration
  - [FIXED] xhookdpr for x86-64 mode

- VM
  - [ADDED] #635 : new entry point to invoke the functionality from another program

- API
  - [FIXED] toJson extension 
  - [FIXED] try statement does not allocate an exception
  - [ADDED] extensions'io:ByteArrayReader,ByteArrayWriter

- SAMPLES
  - [ADDED] #635 : new tutorial to call an ELENA library from C++ / C#

- Tools
  - [ADDED] ecv-cli : adding a filter for listing members
  - [FIXED]elt-cli : fixing support for extension methods

- IDE
  - [FIXED] watch : displaying stack-allocated classes
  - [ADDED] syntax highlighting

## ELENA 6.0.9
*19.04.2024*

- ELENA
  - [ADDED] new opcode - parent
  - [ADDED] meta expression: nested singletons
  - [ADDED] new class flag - elPacked
  - [ADDED] new attribute - packed  

- ELC
  - [ADDED] #637 - unit tests for byRefOp optimization
  - [ADDED] #637 - unit tests for intCopying optimization
  - [ADDED] #610 - struct redesign
  - [FIXED] optimization : copy 8 / copydpn 8
  - [FIXED] #615 : The field is not property incremented by an addition assignment
  - [FIXED] x64 : fround opcode
  - [FIXED] x64 : fabs opcode
  - [FIXED] x86 : xdispatchmr opcode

- API
  - [ADDED] system'dynamic'expressions'LazySymbolExpression
  - [FIXED] ushort - native comparison operations
  - [ADDED] system'net
  - [ADDED] ltests
  - [ADDED] net
  - [ADDED] win64 gui
  - [FIXED] realOp.RoundedInt extension

- SAMPLES
  - [FIXED] interpreter sample for x86-64

- Tools
  - [FIXED] #618 : template class descriptions

- IDE
  - [ADDED] include / exclude file
  - [FIXED] ctrl+w hides the frame
  - [ADDED] ins / over modes
  - [ADDED] dark color scheme
   
- Misc
  - [ADDED] GitHub Action - MSBuild nightly build

## ELENA 6.0.8
*21.03.2024*

- ELENA
  - [ADDED] an opcode LNEG

- ELC
  - [ADDED] native negate operation for long integers
  - [ADDED] #588 - Operation with int constant
  - [ADDED] multi-profiles per a single project file; new option -l<profile-name>
  - [ADDED] #638 - Accessing a variable declared in the upper scope
  - [FIXED] calling a variadic method without arguments directly
  - [FIXED] declaring a template based type with T is an array as a method result
  - [FIXED] GC : perm generation is not included as roots for the partial collect
  - [FIXED] generating a proper debug info for local structure / structure self
  - [FIXED] it has to be possible to convert int literal directly to v_int8 / v_int16 / v_int64, depending on the value size 
  - [FIXED] x86 MTA : starting the new thread / program
  - [FIXED] x86 MTA : GC routine
  - [FIXED] byrefHandler : if it is not contain the explicit return, assign self to byref retVal
  - [FIXED] no need for typecasting operation after if-else operation with both branches containging returning operation
  - [FIXED] optimizing operation with int constants

- API
  - [ADDED] sqlite module
  - [FIXED] string.toUpper() : returns a string with an incorrect length
  - [FIXED] windproc
  - [ADDED] xforms

- SAMPLES
  - [ADDED] sqlite_test
  - [FIXED] Rosetta Code : Interactive programming (repl), Vigenere ciphere
  - [ADDED] c_a_g sample
  - [ADDED] agenda sample
  - [ADDED] graph sample

- Tools
  - [ADDED] support for stdcall for win32

- IDE
  - [ADDED] project settings - select the project profile
  - [FIXED] #634 - IDE debugger: the debugger must hide trace line on wait
  - [FIXED] #634 - debugger step over some expressions
  - [FIXED] #636 - IDE bugs
  - [FIXED] displaying stack-allocated structures
  - [FIXED] warn if the source code is not compiled after the change before run / debug
  - [FIXED] warn if the source code is changed and not save before run / debug
  - [FIXED] the document is in read-only mode during the debugging

## ELENA 6.0.7
*11.02.2024*

- ELENA
  - [ADDED]meta command #load, #clear
  - [ADDED]#563:Extended statement templates
  - [FIXED]#508 - working on multi-value assigning
  - [FIXED]#86 - primitive operations : int + long
  - [FIXED]"this self" for an extension
  - [ADDED]system : Int8Number, UShortNumber
  - [ADDED]#629 - Read-only fields
  - [FIXED]#613 - bytecode optimization
  - [FIXED]#623 - Overriding property set accessor
  - [FIXED]#601 - bytecode optimization
  - [ADDED]evaluate an expression in compile-time if possible
  - [ADDED]#562 - Nullable support

## ELENA 6.0.6
*19.10.2023*

- ELENA
  - [ADDED]snop opcode
  - [ADDED]xfsave opcode
  - [ADDED]opcodes : xcmp sp, load sp
  - [ADDED]$yield operator
  - [ADDED]yieldable method
  - [ADDED]using statements
  - [ADDED]lock statement
  - [ADDED]class constants
  - [ADDED]globals

- ELC
  - [ADDED]MTA support for x86
  - [ADDED]#321,#173 - conditional boxing
  - [ADDED]warn if the outpur type is less accessible than the symbol
  - [ADDED]variadic and normal methods cannot have similar names
  - [ADDED]weak implementations of &&, ||, ^^
  - [FIXED]struct typecast handler : do not copy the struct
  - [FIXED]constructor explicit multi-dispatch handler
  - [FIXED]support constants in array size declaration
  - [FIXED]passing variadic argument to another method
  - [FIXED]#47 : Evaluating an expression in a symbol
  - [FIXED]duplicates in GA_EXT_OVERLOAD_LIST
  - [FIXED]#15 - return double / long from external function

- ELENAVM
  - [ADDED]preloaded symbols
  - [ADDED]loadAddressInfo
  - [FIXED]LoadExtensionDispatcherLA
  - [FIXED]LoadActionNameLA

- ELENASM
  - [FIXED]#366 - Script engine rule not recognized correctly

- API
  - [ADDED]event property template
  - [ADDED]system'winforms module
  - [ADDED]forms module
  - [ADDED]forms'SDIDialog
  - [ADDED]forms'Button
  - [ADDED]forms'Edit
  - [ADDED]forms'Label
  - [ADDED]system'dynamic'ExtensionDispatcher
  - [FIXED]Console class should be public with internal constructor
  - [FIXED]system'io'StreamWriter
  - [FIXED]system'collections'Stack
  - [FIXED]system'dynamic'DynamicTape - variadic functions
  - [FIXED]dynamic expressions

- SAMPLES
  - [ADDED] Conway's Game of Life

- IDE
  - [ADDED]recent open projects
  - [FIXED]debug watch : Reference<int>

- Tools
  - [FIXED] elt-cli

## ELENA 6.0.5
*19.08.2023*

- ELENA
  - [ADDED]implementing corex60 routines
  - [ADDED]constant array of integers
  - [ADDED]native support for ?: operator
  - [FIXED]#524 : Alt expression breaks the compilation
  - [ADDED]message name functionality
  - [ADDED]new opcodes : fiadd, fisub, fimul, fidiv

- ELC
  - [ADDED]basic bytecode optimization rules
  - [ADDED]basic build optimization rules
  - [ADDED]warn on unassigned variables
  - [FIXED]nested class fields
  - [FIXED]assignment operations with integers
  - [FIXED]calling an interface / closed-class method
  - [FIXED]warning on resending to itself (e.g. for constructor)
  - [FIXED]resending to the property
  - [FIXED]template-based extensions
  - [FIXED]template of template
  - [FIXED]do not box the returning self for the stacksafe method

- ELENAVM

- API
  - [FIXED]string comparison routines
  - [FIXED]conversion routines
  - [FIXED]nested exception catching
  - [ADDED]number of new unit-tests
  - [ADDED]system'runtime'commonOp - extension to read package info
  - [ADDED]system'culture
  - [ADDED]system'routines'stex
  - [ADDED]new unit-tests: methodnotfound, nilreference

- SAMPLES
  - [ADDED]24 sample

- IDE
  - [ADDED]Find dialog
  - [ADDED]Goto dialog
  - [ADDED]Toolbar dialog
  - [ADDED]Context menu
  - [FIXED]IDE blinking issues
  - [ADDED]Window menu
  - [ADDED]Unident
  - [FIXED]Running to the cursor
  - [ADDED]Recent files
  - [ADDED]erase line, trim, upper case, lower case
  - [ADDED]About dialog
  - [ADDED]Editor settings
  - [ADDED]Duplicate line
  - [ADDED]close all but active

- Tools
  - [FIXED]API Doc generation
  - [ADDED]ecv-cli: showing the method number
  - [ADDED]ecv-cli: showing the module manifest info

## ELENA 6.0.4b
*27.06.2023*

- ELENE
  - fixing variadic multimethod dispatcher

- ELENAVM
  - (re)implementing loading classes / symbols / messages / message names

- API
  - system'dynamic'DynamicTape
  - system'dynamic'expressions'Expression

- SAMPLES
  - pi sample
  - calc sample

- Tools
  - #553 : elt crash fixed

- Misc  
  - #579 : adding missing files to the installation package 

## ELENA 6.0.3b
*30.05.2023*

- ELENA
  - #508 : multi-value
  - fixing an issue with call stack information : incorrect / missing source code information
  - fixing conversion uint to string and back
  - fixing number of bugs
  - migrating rest of API to ELENA 6.0
  - fix implementation of abstract method with the stack allocated returning value

- IDE
  - ide : fixing go to source routine

## ELENA 6.0.2b
*31.03.2023*

- ELENA
  - fixing a critical error with long number comparison operations
  - (re)implementing Script Engine (elenasm60 dll)
  - #568 : Converting an external function return to the pointer
  - starting work on (re)implementing multi-threading support (win x86)
  - a lot of bug fixes : including critical ones
  - migrating number of examples

## ELENA 5.8
*01.10.2021*

- ELENA
  - binary incompatible due to module structure change / new opcodes / support for amd64
  - #519 : abstract methods should not have a body
  - #521 : Auto generating multi-method default handler with type casting
  - #77 : retiring core 
  - #378 : handle access violation
  - #467 : Raise an out of memory exception 
  - #212 : GC optimization : mg -> yg collecting 
  - #532 : PERM Space 
  - #531 : Dynamic interface mixin 
  - #534 : Generic typecast handler 
  - #535 : Setting several properties in a row 
  - #530 : Supporting unary operators : ! and -
  - supporting real number literal without postfix (e.g. 2.2)
  - #539 : try-catch-finally operator 
  - #538 : Bitwise operations 
  - #540 : support typeof attribute 
  - #541 : Support lazy creating 
- ELC
  - a warning shown when an object cannot be converted into another one
  - a warning shown when an object does not handle a message (for strong-typed ones)
  - #511 : Infinite extension **populate** call
  - #514 : Incorrect warning message for unhandled function message 
  - #516 : Dispatch handle : dispatching to an expression 
  - #517 : Interface dispatcher : Check if all abstract methods are implemented 
  - #520 : Parent multi-method default handler is not invoked by its children 
  - #526 : dispatch operator should work with expression as well 
  - #523 : Typecast the argument list to the expected ones and call it directly if there is only a single strong-typed method
- IDE
  - several usability bugs fixed
- TOOLS
   - #323 : Run ELENA code without build

## ELENA 5.0
*14.07.2020*

- ELENA
  - binary incompatible due to major version change
  - Explicit namespaces
  - Flat library hierarchy
  - nested code brackets
  - #500 : Nested code blocks
  - #492 : Yield statement 
  - #380 : Run-time extension dispatching 
  - #494 : Method result multi-dispatching 
  - #495 : Inheritable static methods 
  - #501 : Modificator **new** can be used with a variable
- ELC
  - deep code refactoring - treamlining code generation, increasing the compilation speed
  - #489 : Creating a variadic argument enumerator
  - #490 : Passing a variadic argument list in sub call
  - #491 : The message target is corrupt when a variadic argument is used in sub call 
  - #381 : const static property
  - #162 : compiler redux - embedded constructor call 
  - #505 : constructor with variadic argument list 
  - #498 : Compile-time dispatching nil argument 
  - #472 : Embeddable property is not called correctly
