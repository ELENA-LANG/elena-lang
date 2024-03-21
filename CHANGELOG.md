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
