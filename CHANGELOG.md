## ELENA 6.8.0

- ELENA
  - [!ADDED] a new dialect EL7
  - [ADDED] direct extension template call
  - [ADDED] the explicit lambda function returning value
  - [ADDED] #606 : parameterized templates
  - [ADDED] meta programming : supporting #for / #endfor statement
  - [ADDED] invoking get property with a message name literal
  - [ADDED] intermediate local variables (aka object shortcuts)
  - [ADDED] #562 : nullable
  - [ADDED] new attribute "__nonboxable" - requiring only memory allocated objects
  - [ADDED] shorthand syntax for lambda function without arguments : ([] => "Hello from Func")
  - [ADDED] "#else" statement

- ELC
  - [ADDED] option "-xn-" to turn off nullable types
  - [ADDED] option "-xtwin32" to support cross-platform compilation
  - [ADDED] option "-xtwin64" to support cross-platform compilation
  - [FIXED] aarch64 : xlabeldp opcode
  - [ADDED] aarch64 : fsindp / fcosdp / fp opcodes
  - [FIXED][CRITICAL] x86 : lloaddp opcode
  - [FIXED] suppress a method not found warning when calling itself
  - [ADDED] #781 : Cross-compile on Windows for Linux 
  - [ADDED] #778 : Cross-compile from Unix to Windows 
  - [ADDED] warning if the inherited method has different nullable signature than the parent one
  - [CRITICAL][FIXED] __intermediate variable
  - [ADDED] warning if the target is a structure for ?. / !. operations
  - [CRITICAL][FIXED] resolving a template compiled in a third-part module
  - [FIXED]an issue with a template-based field of the structure
  - [FIXED] duplicate boxing / unboxing
  - [FIXED] correct boxing / unboxing in async operations
  - [ADDED] #818 : Support <?xml version="1.0"?> in .prj file
  - [DONE] #824 : Improving usability : making some error / warning messages more clear
  - [FIXED] in-place constructor is missing
  - [ADDED] support property call shorthand syntax
  - [FIXED] displaying user friendly error for an incompatible closure function

- API
  - [FIXED] textgen : support {{ }} special symbols 
  - [ADDED] new inline operator - ":sizeof"
  - [FIXED] int to string conversion routine for IntNumber.MinValue
  - [ADDED] stringListOp.splitByNewLine
  - [ADDED] wideListOp.splitByNewLine
  - [ADDED] system'PropertyMessageName
  - [ADDED] record template
  - [REDUX] nilValue => NilValue
  - [CRITICAL][FIXED] CountDownEvent
  - [ADDED] #35 : adding net'http'server module
  - [ADDED] #35 : adding webapi module
  - [ADDED] UnsafeArray, UnsafeArray<T>

- SM

- Scripts

- SAMPLES

- IDE
  - [FIXED] an issue with a vertical splitter
  - [FIXED] breakpoint must be inside the loop
  - [FIXED] step over the loop
  - [ADDED] Project Settings : Warning level combobox
  - [FIXED] closure : should display captured variables / self
  - [FIXED] #802 : The horizontal scroll bar is broken
  - [FIXED] comment highlighting 
  - [FIXED] step over multi-conditional if statement
  - [ADDED] supporting project with profiles
  - [FIXED] active bracket highlighting 

- Tools
  - [ADDED] elt-cli : supporting textgen
  - [FIXED] ecv-cli - support toggling pagination

- Misc
  - [ADDED] #763 : freebsd nightly build

## ELENA 6.7.0

- ELENA
  - [ADDED] user defined type alias
  - [ADDED] supporting auto field
  - [ADDED] async action / async program entry
  - [ADDED] "use" statement to declare a short cut
  - [ADDED] "#import" statement to register the external library name
  - [ADDED] project variables
  - [ADDED] Conditional compilation

- ELC
  - [ADDED] new option "-xj" to turn on jump alignment
  - [ADDED] if:nil / if:not:nil / if:not template statements
  - [ADDED] :break / :continue / :yield / :await template operators
  - [FIXED] nillable output type - returning nil may lead to a nil operation exception
  - [FIXED] an expression: s ?? String.MinValue
  - [FIXED] template generation - internal operation is not correctly recognized
  - [FIXED] generating unnecessary wrapper classes while validating byref handlers
  - [FIXED] generating async methods - operation with class fields
  - [FIXED] constant symbols can be used to define the array field size
  - [FIXED] calling extension method if internal one exists
  - [REDUX] CORE_TLS_INDEX is no longer user for x86 / x86-64
  - [FIXED] #702 : Elena 6.5.0 crashes in Ubuntu 24.04 docker image
  - [FIXED] an invalid operation with a structure field
  - [FIXED] linux x86-64 : debug module structure
  - [FIXED] Linux x86-64 : FPU operations
  - [FIXED] ppc64le extopen / extclose opcode
  - [FIXED] #704 : Porting to FreeBSD/PowerPC64
  - [FIXED] aarch64 extopen / extclose opcode
  - [FIXED] #283 : unboxing duplicate object
  - [ADDED] CORE_MATH_TABLE core table
  - [ADDED] ppc64le : fexp
  - [ADDED] aarch64 : fexp
  - [ADDED] ppc64le : fln
  - [ADDED] aarch64 : fln
  - [FIXED] #283 : unboxing duplicate object
  - [FIXED] default built-in variable inside the constructor
  - [FIXED] GC routine in MTA
  - [FIXED] using inside using in async / yield method
  - [FIXED] string interpolation with character codes
  - [FIXED] ppc64le : fiadd, fisub, fimul, fidiv opcodes
  - [ADDED] project collection : support sub folders 
  - [FIXED] AARXH64 : fiadd, fisub, fimul, fidiv
  - [ADDED] supporting base_path attribute in a project collection
  - [ADDED] supporting profile attribute in a project collection
  - [ADDED] #742 : Declaring external callback 
  - [ADDED] #748 : Supporting FreeBSD 
  - [ADDED] #756 : supporting mingw toolset

- API
  - [ADDED] net'http'HttpClient, net'http'Uri
  - [ADDED] system'text'parsing'RegEx
  - [ADDED] system'net'NetworkStream
  - [ADDED] system'net'Socket : poll<Timespan,SelectMode>[3]
  - [ADDED] net'TcpListener : prop:Pending
  - [ADDED] forms'Combobox
  - [ADDED] net'HttpClient
  - [FIXED] system'threading'Task - raise an exception if the task was completed before
  - [FIXED] BlockingQueue<T> implementation
  - [ADDED] new library : mbedtls 
  - [FIXED] VariadicExtensionDispatcher : dispatcher
  - [ADDED] system'routines'CountDownEnumerator
  - [ADDED] extension countDown
  - [ADDED] BaseEdit.onChange event
  - [ADDED] algorithms'InsertSortAlgorithm, algorithms'InsertSortAlgorithm<T>
  - [ADDED] algorithms'BinarySearchAlgorithm<T>
  - [ADDED] system'UnsafePointer<T>
  - [FIXED] system'io'MemoryStream - reading operation
  - [FIXED] ByteArrayExConvertor.toByteArray extension method
  - [ADDED] new library : textgen
  - [ADDED] system'dynamic'expressions : ForLoopExpression
  - [ADDED] net'http'HttpClient : post, postAsync
  - [ADDED] system'text : Base64Encoding, Base64Encoder

- SM
  - [ADDED] supporting $regex rules

- Scripts
  - [ADDED] xforms : supporting RadioButton, Panel, Edit, Combobox
  - [ADDED] LScript : for loop

- SAMPLES
  - [ADDED] console regex sample
  - [ADDED] net : httpget sample
  - [FIXED] ppc64le : pi sample
  - [FIXED] aarch64 : pi sample

- IDE
  - [FIXED]debugger : step over multi-select statement
  - [ADDED] Linux - open a file 
  - [FIXED] #716 : applying highlight setting changes immediately
  - [FIXED] #716 : highlight setting os on by default
  - [FIXED] fix syntax highlighting with character constants
  - [FIXED] #717 : The setting to change code editor font size doesn't work 
  - [FIXED] #715 : There is no options available to change the code editor font
  - [FIXED] displaying short struct field
  - [ADDED] #721 : Dragging files into the IDE to open doesn't work
  - [ADDED] #723 : include path to approot temporally inside IDE
  - [ADDED] #722 : [IDE] Scrolling experience
  - [ADDED] #725 : [IDE] large toolbar buttons
  - [FIXED] highlighting a string containing character codes
  - [FIXED] saving IDE config file in the correct folder
  - [FIXED] supporting clipboard operation without selection
  - [FIXED] IDE64 : fixing undo / redo operations
  - [ADDED] auto indent
  - [FIXED] an outage is not being calculated correctly
  - [ADDED] brackets highlighting

- Tools
  - [ADDED][LDOC] static methods are in the separate category

## ELENA 6.5.0

- ELENA
  - [FIXED] method reference : support function singletons
  - [FIXED] direct typecasting must have priority over implicit conversion
  - [ADDED] Thread static symbols / static fields
  - [ADDED] new byte-codes : peektls / storetls
  - [FIXED] ?? operator
  - [ADDED] static constructor
  - [ADDED] internal symbols
  - [ADDED] supporting async methods
  - [ADDED] supporting indexed methods
  - [ADDED] indexed internal methods can be declared in a closed class
  - [REDUX]!! refactoring grammar to make it more readable
  - [ADDED] support parameterized extension method call

- ELC
  - [ADDED] new option -el5 / -el6 (default) - specifying grammar compatible to ELENA 5.x or 6.x
  - [ADDED] Win x32-64 : MTA Console
  - [FIXED] inline field assignment in a template class
  - [FIXED] return inside lock
  - [FIXED] ppc64le : fix iteratorMethodTest
  - [FIXED] declaring a lambda function with template based argument
  - [FIXED] calling a method directly with nil argument when allowed
  - [FIXED] single dispatch of a private / internal / protected method
  - [FIXED] closure argument types can be specified by an expected type
  - [FIXED] it is not possible to declare internal default constructor
  - [FIXED] #675 - Using ++ operator with a weak type
  - [FIXED] incorrect typecast handler signature for the classes inheriting template-based ones
  - [FIXED]variadic argument list of super class is type-casted
  - [FIXED] #689: declaring a lambda function with template based argument
  - [FIXED] fixing issues ABI convention for external calls
  - [FIXED] preloaded symbols in sub namespaces
  - [FIXED] calling indexed method from the sealed class
  - [ADDED] new option -xs - Strict type enforcing option (raise an error if the sealed class is typecasted / called the method that does not exists)

- API
  - [ADDED] system'threading'Thread: Priority, join<int>, Current
  - [ADDED] system'runtime'Environment
  - [ADDED] system'collections'threadsafe'ConcurrentQueue<T>
  - [ADDED] #154 : system'threading'Semaphore
  - [ADDED] system'threading'BlockingQueue<T>
  - [ADDED]system'threading'ThreadPool
  - [ADDED] #154 : system'threading: ManualResetEvent, AutoResetEvent, CountDownEvent
  - [ADDED] system'threading : Task
  - [ADDED] system'io'Directory : static getFiles / getFiles<string>
  - [ADDED] File : saveContent, readContent, readWideContent
  - [ADDED] system'io'threading : asyncStreamOp extension
  - [ADDED] system'threading.Task - sleep<int>
  - [ADDED] extensions'threading : outputConcurrentOp extension
  - [REDUX] all symbols must start with a capital letter
  - [ADDED] system'threading : Task<TResult>
  - [FIXED] system'objectOp.__getClassName[1]
  - [ADDED] extensions'routines'stex : toArray<T>

- SAMPLES
  - [ADDED] threadpool
  - [ADDED] tasks
  - [ADDED] asyncsamples
  - [ADDED] task sample 2

- Tools
  - [ECV][ADDED] displaying class attributes
  - [FIXED][ldoc] system-threading-.html must not be generated
  - [FIXED][ldoc] system-threading : generate only templates declared in the module
  - [FIXED][ASMC] opcode mov gs,r/m64

- IDE
  - [FIXED] #679 : class already exist
  - [FIXED] debug source path
  - [ADDED] it is possible to set the target type for a single file project
  - [FIXED] project options - debug command arguments
  - [FIXED] correct debug info of the constructor self variable
  - [FIXED] #413 - boxing variadic argument

- Misc
  - [ADDED]new functional test : script_tests

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
