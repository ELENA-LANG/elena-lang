Content
========

+ [Overview](#overview)
+ [Installing](#installing)
+ [Language Elements](#language-elements)
  + [Entry point](#entry-point)
  + [Variables](#variables)
  + [Numbers](#numbers)
    + [Integer Types](#integer-types)
    + [Floating-point types](#floating-point-types)
    + [Numeric literals](#numeric-literals)
    + [Numeric conversions](#numeric-conversions)
  + [Mathematical Operations and Functions](#mathematical-operations-and-functions)
    + [Arithmetic Operators](#arithmetic-operators)
    + [Bitwise Operators](#bitwise-operators)
    + [Assignment Operators](#assignment-operators)
    + [Numeric Comparisons](#numeric-comparisons)
    + [Operator Precedence](#operator-precedence)
    + [Numerical Conversions](#numerical-conversions)
    + [Methods and functions](#methods-and-functions)
  + [Strings](#strings)
    + [Characters](#characters)
    + [String literals](#string-literals)
    + [Reading string content](#reading-string-content)
    + [Basic operations and functions](#basic-operations-and-functions)
    + [Conversion to and from strings](#conversion-to-and-from-strings)
  + [Functions](#functions)
    + [Returning operator](#returning-operator)
    + [Closures](#closures)
    + [Strong-typed arguments](#strong-typed-arguments)
    + [Message functions](#message-functions)
  + [Control Flow](#control-flow)
    + [Boolean type](#boolean-type)
    + [Branching operator](#branching-operator)
    + [Branching statements](#branching-statements)
    + [Looping statements](#looping-statements)
    + [Exception handling](#exception-handling)
  + [Symbols](#symbols)
    + [Values / Constants](#values-/-constants)
    + [Static symbols](#static-symbols)
    + [Preloaded symbols](#preloaded-symbols)
  + [Types](#types)
    + [Primitive types](#primitive-types)
    + [Classes](#classes)
    + [Abstract classes](#abstract-classes)
    + [Interfaces](#interfaces)
    + [Singletons](#singletons)
    + [Structs](#structs)
    + [Strings](#strings)
    + [Extensions](#extensions) 
    + [Sealed / Closed Classes](#sealed--closed-classes)
  + [Fields](#fields)
    + [Class Fields](#class-fields)
    + [Field Initializer](#field-initializer)
    + [Constant fields](#constant-fields)
    + [Static fields](#static-fields)
    + [Static constant fields](#static-constant-fields)
    + [Static sealed fields](#static-sealed-fields)
    + [Static attribute fields](#static-attribute-fields)
    + [Primitive fields](#primitive-fields)
    + [Fixed size array field](#fixed-size-array-field)
    + [Dynamic size array field](#dynamic-size-array-field)
  + [Methods](#methods)

## Overview
A programming language is a mammoth task to develop and to learn. So encountering a new language you may ask: why another
programming language? The short answer is to try it a little differently. Why not to treat the message like a normal
language element with which different operations are possible : loading, passing, modifying, dispatching? Could we do it without using reflection all the way? This was my motivation. ELENA is designed to facilitate writing dynamic, polymorphic code, combing elements of dynamic and static languages.

ELENA is a general-purpose language with late binding. It is multi-paradigm, combining features of functional and object-oriented programming. It supports both strong and weak types, run-time conversions, boxing and unboxing primitive types, direct usage of external libraries. Rich set of tools are provided to deal with message dispatching : multi-methods, message qualifying, generic message handlers. Multiple-inheritance can be simulated using mixins and type interfaces. Built-in script engine allows to incorporate custom defined scripts into your applications. Both stand-alone applications and Virtual machine clients are supported

To summarize all of the above, let's name some functions of ELENA:
  * Free and open source (MIT licensed)
  * Complete source code
  * Unicode support (utf-8)
  * GUI IDE & Debugger
  * Optional types
  * Multiple dispatching / multi-methods
  * Support of variadic methods
  * Yieldable methods
  * Closures
  * Mixins
  * Type interfaces / conversions
  * Class / code templates 
  * Script Engine

## Installing

The latest version of the language can be always found at [GitHub](https://github.com/ELENA-LANG/elena-lang/releases) or [Sourceforge](https://sourceforge.net/projects/elenalang/files/). 

The simplest way to install the language is to use a setup. The zip files are provided as well. To install from them, just unzip all the files into a directory you want. You will need to add a path to BIN folder to system environment (e.g. &lt;app root&gt;\bin). The language requires Visual C++ Redistributable for VS 2017.

To open, compile or debug the programs and libraries use ELENA GUI IDE (&lt;app root&gt;\bin\elide.exe) or ELENA Command Line Compiler (&lt;app root&gt;\bin\elc.exe).

In ELENA IDE you may select **File->Open->Open Project** option and open an appropriate project file (*.prj). Then select **Project->Compile** option to compile the project, **Debug->Step Over** or **Debug->Step Into** to debug it or **Debug->Run** to execute the program.

A number of tutorials with complete source code can be found [here](https://github.com/ELENA-LANG/tutorials)

## Language Elements

We will start to learn ELENA by getting familiar with the basic program elements : programs, variables, symbol, classes, control flows.

Any ELENA program consists of source files (with **.l** extension) and the main project file (with **.prj** extension). The project file contains the main information how the source files will be compiled and what will be the result of the compilation: a library or an executable. The simplest project type is console one : it will generate a stand-alone console program.

The easiest way to generate a new project is to use IDE. Let's select **File->New->Project**

![New Project Dialog](https://user-images.githubusercontent.com/5677187/65635302-5c8e2a80-dfe0-11e9-9059-376a9ec779fb.PNG)

We have to choose the project type - console, enter the root namespace - example1 and specify the target file name - example1.

The project should contain at least one source file - **File->New->Source File**. After saving the project we are ready to write our first program.

### Entry point

A program entry is a main program code which is called at the program startup (after some preparations). When the last statement is executed the program is terminated. Any exception happens in the main code (or in nested one) will be handled by the system handler allowing graceful program exit.

To create an entry point we have to declare a public function (or to put it more exactly - a singleton closure) with a name **program** and no arguments in the project root namespace (in the simplest case in any of the project source files). Writing a function is quite straightforward for anyone familiar with C-like languages:

    public program()
    {
    }

The code is written inside curly brackets and consists of statements. A statement has to be terminated with semicolon (except the last one, where the semicolon is optional). A typical statement is a method call (a massage sending), an operation or a control-flow statement. In our case we will print a text on the system console using **console** symbol. The message argument is a string constant (enclosed in double quotes):

    public program()
    {
       console.writeLine("Hello World!");
    }

The output will be:

    Hello World! 

### Variables

A variable is a named place in the program memory (more exactly in the call stack) which contains a value. In the simplest case it is a reference to an object or null pointer - **nil** value. A variable name may contain any UTF-8 symbols (except special ones, white-spaces; the leading character cannot be a digit) and is case-sensitive. There are no reserved keywords. Variables with duplicate names (declared in the same scope) are not allowed. 

    public program()
    {
        var myFirstVariable := "Hello";
        var Δ := 1;
    }

The declaration starts with an *attribute* **var**, the bound value are placed after an assigning operator (*NOTE: 
assigning and equivalent operators are not the same in ELENA*). The value may be the result of the expression as well.

The declaration and assigning can be separated:

    public program()
    {
        var myFirstVariable;
        myFirstVariable := "Hello";
        var Δ := 1;
        Δ := Δ + 1;
    }

If a variable is declared without an assignment, its value is **nil**.

A variable type is optional in ELENA. If the type is not explicitly stated the variable is type-less (or more exactly its type is **system'Object**, the super class). It means the type is not checked and any value can be assigned.

        var v := "Hello";
        
        v := 3;

In reality there are many situations when we do care about the variable types. So let's create a strong typed variable. The required type (a class name or alias) should be written before the variable name:

        var string v := "Hello";

where **string** is an alias of **system'String** class.

**var** attribute in this case is optional and we could simplify the code:

        string v := "Hello";

The variable type restricts the possible values of the variable. The biggest difference with statically-typed languages that the type-check happens in **run-time**. So for example the following code can be compiled but will generate a run-time error:

    public program()
    {
        var o := 2;
        string v := o;
    }
  
and the output is:

    system'IntNumber : Method #cast[0] not found
    Call stack:
    system'Exception#class.new[1]:exceptions.l(96)
    system'MethodNotFoundException#class.new[1]:exceptions.l(1)
    system'MethodNotFoundException#class.new[2]:exceptions.l(190)
    system'$private'entry.#invoke[0]:win32_app.l(42)
    mytest'program.#invoke[0]:test.l(6)
    system'$private'entry.#invoke[0]:win32_app.l(37)
    system'#startUp:win32_app.l(55)

The value can be successfully converted if an appropriate conversion method is declared:

    var o := 2;
    real r := o;

In this case **system'IntNumber** class supports a conversion to **system'RealNumber** type.

The variable type can be automatically deduced from its initializer using **auto** attribute:

    auto r := 1.2r;

the variable type, **system'RealNumber** (or **real**), is defined by an assigned value - a numeric literal - **1.2r**

### Numbers

Numbers are one of the most essential data types of a programming language. ELENA supports both integers and floating-point numbers. These are:

#### Integer Types

Alias | Type | Signed? | Number of bits | Smallest value | Largest value
----- | ---- | ------- | -------------- | -------------- | --------------
byte | system'ByteNumber | No | 8 | 0 | 2^8 - 1
short | system'ShortNumber | Yes | 16 | -2^15 | 2^15-1
int | system'IntNumber | Yes | 32 | -2^31 | 2^31-1
uint | system'UIntNumber | No | 32 | 0 | 2^32-1
long | system'LongNumber | Yes | 64 | -2^63 | 2^63-1

#### Floating-point types

Alias | Type | Number of bits
----- | ---- | --------------
real | system'RealNumber | 64  

Numbers are primitive built-in types. They could be stored directly in the program memory or used as immediate values (numeric literals). Arithmetic and relational operations are natively supported. Nevertheless, thanks to just-in-time boxing they can be used as normal objects without paying much attention:

    import extensions;

    public program()
    {
       int n := 2;
       console.printLine(n);
    }

The result is    

    2

In the code above the first operation is done directly (copying immediate value into the memory) and the second one uses a boxed value (boxing a number into **system'IntNumber** object and passing it to an extension method **extensions'outputOp.printLine[]**). The appropriate types are called primitive wrappers. When it is required the wrapper value can be unboxed back into the memory.

#### Numeric literals

ELENA supports the following numeric literals:

       int  n1  := 2;             // a 32bit integer literals
       int  n2 := -23; 
       uint x  := 0Fh;            // an unsigned 32bit hexadecimal integer        
       long l1 := 1234567890123l; // a 64bit integer literals
       long l2 := -23354436l345l;
       real r1 := 23.2r;          // a floating-point literals
       real r2 := 1.2e+11r;
    
#### Numeric conversions

Numbers can be implicitly converted into each other. We may assign an integer to a long integer or a floating-point number. But the opposite is not always possible.

        long l := 123l;
        real r := 123.0r;
        
        int n1 := l; // works
        int n2 := r; // fails        

For unsigned, short or byte integers, both conversions are possible with a loss of information in some cases.

        uint x := 0FFFFFFFEh;
        int  n := 300;
        
        short s := n; // works
        byte  b := n; // works, but the value is 44
        int  sn := x; // works, but the value is -2 

### Mathematical Operations and Functions

ELENA provides a basic set of operations with primitives. It is easily extended with a help of methods declared in appropriate classes and extensions.

#### Arithmetic Operators

The following arithmetic operators are supported on all primitive numeric types:

Expression | Name  
---------- | ---- 
x + y | Addition operator
x - y | Subtraction operator
x * y | Multiplication operator
x / y | Division operator

The operands may be of different types. The result of the arithmetic operation, in this case, is the biggest operand type (byte < short < int < long < real). E.g. when we sum an integer and a floating-point numbers the result is floating-point. The division of two integer types are always an integer. If one of the operands is a floating-point number the division is floating point one. 

    console
       .writeLine(3*4)
       .writeLine(3*4.0r)
       .writeLine(5/2)
       .writeLine(5.0r/2);

The result is:

    12
    12.0
    2
    2.5

Several operation can be used together:

     console.writeLine(1+2*3.5r-3l/2);
 
#### Bitwise Operators

Bitwise operators are supported only by integer numbers.

Expression | Name  
---------- | ---- 
x && y | bitwise **and** operator
x \|\| y | bitwise **or** operator
x ^^ y | bitwise **xor** operator
x $shl y | bitwise shift left operator
x $shr y | bitwise shift right operator

Similar the different integer types can be used and the result is the biggest operand type.

    console
       .writeLine(4 && 2)
       .writeLine(3 || 10000000000l)
       .writeLine(5 ^^ 3)
       .writeLine(5 $shl 2)
       .writeLine(5 $shr 1);

The result is:

    0
    10000000003
    6
    20
    2

#### Assignment Operators

There are four arithmetic assignment operators to simplify the operations with the left hand operands:

Expression | Name  
---------- | ---- 
x += y | add and assign operator
x -= y | subtract and assign operator
x *= y | multiply and assign operator
x /= y | divide and assign operator

For example

    x += y

is a shorthand for
 
    x := x + y

The left-hand operand should always be a variable.

#### Numeric Comparisons

Standard comparison operations are defined for all the primitive numeric types:

Expression | Name  
---------- | ---- 
x == y | equality
x != y | inequality
x < y | less than
x > y | greater than
x <= y | less than or equal to
x >= y | greater than or equal to

The result of a comparison is Boolean values : **true** or **false**.

    import extensions;
      
    public program()
    {
        console
           .printLine(2.0r == 2)
           .printLine(2 == 3)
           .printLine(2 < 3)       
           .printLine(2 > 3)
    }

The results are:

    true
    false
    true
    false

#### Operator Precedence

Operators are ordered in the following manner, from highest precedence to lowest:

Category | Operators
-------- | ---------
Multiplication | *, /, $shl, $shr
Addition | +, -
Comparisons | ==, !=, <, >, <=, >=
Bitwise | &&, \|\|, ^^

#### Numerical Conversions

Numbers can be converted using either implicit or explicit conversions. Implicit conversion is done automatically but part of the information may be lost. Explicit conversions check if the value will be overflown and generate exceptions.

The recommended way is to use conversion extension methods declared in **extensions** module

Method Name | Description
----------- | -----------
toByte()    | converts the target to system'ByteNumber type
toShort()   | converts the target to system'ShortNumber type
toInt()     | converts the target to system'IntNumber type
toUInt()    | converts the target to system'UIntNumber type
toLong()    | converts the target to system'LongNumber type
toReal()    | converts the target to system'RealNumber type

Here several examples:

    import extensions;
    
    public program()
    {
        int n := 3;
        
        byte  b := n.toByte();
        short s := n.toShort();
        long  l := n.toLong();
        real  r := n.toReal();  
    }

If a conversion leads to an overflow an exception is generated:

    int n := 300;
    
    byte  b := n.toByte();

The output will be:

    An index is out of range
    Call stack:
    system'Exception#class.new[1]:exceptions.l(96)
    system'OutOfRangeException#class.new[1]:exceptions.l(126)
    system'OutOfRangeException#class.new[0]:exceptions.l(126)
    extensions'convertorOp.auto#toByte[1]:convertors.l(383)
    mytest'program.#invoke[0]:test.l(16)
    system'$private'entry.#invoke[0]:win32_app.l(37)
    system'#startUp:win32_app.l(55)

#### Methods and functions

The representative classes greatly extends a set of primitive operations. The functionality is declared either in the proper class or in one of its extensions. For convenience the functions can be used as well.

**Operations with bit-wise masks**

Method | Description
------ | -----------
allMask(mask) | Returns true if all the mask bits are set
anyMask(mask) | Returns true if any of the mask bits are set

For examples the code:

    import extensions;
    
    public program()
    {
        console
            .printLine("8.anyMask(15)=", 8.anyMask(15))
            .printLine("8l.allMask(15)=", 8l.allMask(15))
    }

will generate the following result:

    8.anyMask(15)=true
    8l.allMask(15)=false

**Operations with the number sign:**

Method | Description
------ | -----------
Absolute | Returns the absolute value
Inverted | Returns the inverted value
Negative | Returns the negated value

These properties can be used to return negative, positive or inverted values:

    import extensions;
    
    public program()
    {
        int r := -123;
        
        console
            .printLine(r,".Inverted = ", r.Inverted)
            .printLine(r,".Negative = ", r.Negative)
            .printLine(r,".Absolute = ", r.Absolute)
    }

The result is:

    -123.Inverted = 122
    -123.Negative = 123
    -123.Absolute = 123

When we have to check if our number is positive, negative, zero, odd and so on, these extension methods will be useful:

Method | Description
------ | -----------
isOdd() | Returns true if the number is odd, otherwise false           
isEven() | Returns true if the number is even, otherwise false           
isZero() | Returns true if the number is zero, otherwise false
isPositive() | Returns true if the number is positive, otherwise false      
isNegative() | Returns true if the number is negative, otherwise false       
isNonnegative() | Returns true if the number is non negative ( >= 0), otherwise false    

The usage is quite straightforward:

    import extensions;
    
    public program()
    {
        int n := 2;
        
        console
            .printLine(n," is odd : ", n.isOdd())
            .printLine(n," is even : ", n.isEven())
            .printLine(n," is zero : ", n.isZero())
            .printLine(n," is positive : ", n.isPositive())
            .printLine(n," is negative : ", n.isNegative())
    }

with the following result:

    2 is odd : false
    2 is even : true
    2 is zero : false
    2 is positive : true
    2 is negative : false

**Modulo and real division of integer numbers**

Method | Function | Description
------ | -------- | -----------
mod(operand) | modulo(loperand,roperand) | An Integer remainder
realDiv(operand) | A float-based division 

A modulo operation is implemented in ELENA with a help of an extension method **mod**

    import extensions;
    
    public program()
    {
        console.printLine("5 % 2 = ", 5.mod(2))
    }

The result is as expected one:

    5 % 2 = 1

Instead of extension method we may use an appropriate function declared in **extensions'math** module

    import extensions;
    import extensions'math;
    
    public program()
    {
        console.printLine("5 % 2 = ", modulo(5, 2))
    }

A fraction of two integer numbers is always an integer. If we need an exact result we may use **realDiv** extension:

    import extensions;

    public program()
    {
        console.printLine("5 / 2 = ", 5.realDiv(2))
    }

The output will be:

    5 / 2 = 2.5

**Operation with floating-point numbers**

Method | Function | Description
------ | -------- | -----------
Rounded | |  Returns the rounded number           
RoundedInt | | Returns the rounded integer number        
Integer | | Truncates the fraction part
IntegerInt | | Truncates the fraction part and returns an integer 
frac() | frac(operand) | Returns only the fraction part
ceil() | ceil(operand) |  returns the smallest integer that is greater than or equal to the operand
floor() | floor(operand) | the largest integer that is smaller than or equal to the operand
truncate(precision) | truncate(operand,precision) | Rounds the number to the provided precision
Reciprocal | | returns a number obtained by dividing 1 by operand

We may use either extension methods (declared in **system'math**) or functions (declared in **extensions'math**):

    import extensions;
    import system'math;
    import extensions'math;
    
    public program()
    {
        console
            .printLine("foor(5.6)=", floor(5.6r))
            .printLine("ceil(5.6)=", 5.6r.ceil())
    }

The code produces the following output:

    foor(5.6)=5.0
    ceil(5.6)=6.0

**Mathematical functions**

Method | Function
------ | --------
power(operand) | power(loperand,roperand)
sqr() | sqr(operand)
sqrt() | sqrt(operand)
exp() | exp(operand)
ln() | ln(operand)
sin() | sin(operand)
cos() | cos(operand)
tan() | tan(operand)
arctan() | arctan(operand)
arcsin() | arcsin(operand)
arccos() | arccos(operand)
log2() | log2(operand)
log10() | log10(operand)

Similar both extension methods and functions can be used:

    import extensions;
    import system'math;
    import extensions'math;
    
    Pi = RealNumber.Pi;
    
    public program()
    {
        console
            .printLine("sin(π/3) = ",(Pi / 3).sin())
            .printLine("cos(π/3) = ",cos(Pi / 3))
            .printLine("tan(π/3) = ",tan(Pi / 3))
    }

The result is:

    sin(π/3) = 0.8660254037844
    cos(π/3) = 0.5
    tan(π/3) = 1.732050807569

**Converting to radians, degrees**

Method | Description
------ | -----------
Radian | Converts to radians
Degree | Converts to degree

### Strings

**String** is a sequence of characters representing some text information. The way how to interpret these characters depends on the text encoding. ELENA supports both UTF-8 (**system'String**, or **string** alias) and UTF-16  (**system'WideString**,or **wide** alias) encoded strings. 

Strings are immutable. To modify its content a new string should be created.

A string can be considered as a partial array of UTF-32 characters. It means that for some indexes no character can be returned (an exception will be generated). The reason is UTF-8 / 16 encoding (the character may take more more than one element in the array). When our text is plain ASCII it makes no difference. But for example Russian (for a string) or Chinese (for a wide string) we have to take this feature into account.

#### Characters

A character is a 32-bit primitive value represented by **system'CharValue** class (**char** alias). It can be converted into a 32 bit integer number and vice versa. It supports comparison operations both with another characters and with numbers.

    import extensions;
     
    public program()
    {
        auto s := "♥♦♣♠";    
        
        char ch := s[0];
        int  code := ch.toInt();
        
        console.printLine(ch);
        console.printLine(code);
        console.printLine(ch == code);
    }

The result is:

    ♥
    9829
    true

ELENA supports character literal constant as well:

    console.printLine($9829); // character literal

which will be printed like this:

    ♥

#### String literals

The string literals are enclosed in double quotes:

    string stringConstant := "Привет Мир"; // UTF-8 encoded constant
    wide wideConstant   := "你好世界"w;     // UTF-16 encoded constant

If the string itself contains double quote it should be doubled:

    console.printLine("The string may contain a symbol """);

The output is:

    The string may contain a symbol "

Multi-line string is supported as well:

    console.printLine("This is a string with
    two lines");

The result:

    This is a string with
    two lines

An empty string may be a string literal without content or a constant **emptyString**:

    console.printLine("");
    console.printLine(emptyString);

#### Reading string content

If you want to extract a character from a string, you index into it using an array operator. The index of the first element is 0.

Let's print a first, second and the last characters of the given string. As it was said above for the plain English text, it is quite straight-forward:

    import extensions;
    
    public program()
    {
        auto s := "Hello";    
        
        console.printLine(s[0]); // printing the first element
        console.printLine(s[1]); // printing the second element
        console.printLine(s[s.Length - 1]); // printing the last element
    }

The output is:

    H
    e
    o

Let's try it with Russian word:

    auto s := "Привет";    
    
    console.printLine(s[0]); // printing the first element
    console.printLine(s[1]); // printing the second element
    console.printLine(s[s.Length - 1]); // printing the last element

There error is raised for the second element:

    П
    An index is out of range
    Call stack:
    system'Exception#class.new[1]:exceptions.l(96)
    system'OutOfRangeException#class.new[1]:exceptions.l(126)
    system'OutOfRangeException#class.new[0]:exceptions.l(126)
    system'String.at[1]:memory.l(1356)
    mytest'program.#invoke[0]:test.l(8)
    system'$private'entry.#invoke[0]:win32_app.l(37)
    system'#startUp:win32_app.l(55)

Why? Because in UTF-8 a russian character is encoded with two bytes (two elements of String array). It means that the second character must be read from third, and not from the second position.

We may actually fix the problem using UTF-16. Now every symbol is encoded in only one array element.

    auto s := "Привет"w;    
    
    console.printLine(s[0]); // printing the first element
    console.printLine(s[1]); // printing the second element
    console.printLine(s[s.Length - 1]); // printing the last element

And the output is:

    П
    р
    т

But for the Chinese word it will not work.

To correctly read the next character, we have to use a character **Length** (for strings) or **WideLength** (for wide strings) properties:

    auto s1 := "Привет";    
    auto s2 := "你好世界"w;    
    
    console.printLine(s1[s1[0].Length]);     // printing the second element of UTF-8 string
    console.printLine(s2[s2[0].WideLength]); // printing the second element of UTF-16 string

Enumerators can be used to simplify the task:

    auto s := "Привет";    
    auto it := s.enumerator();   // creating a string enumerator

    it.next();                   // going to the first element
    console.printLine(it.get()); // printing the current element  
    it.next();                   // going to the second element
    console.printLine(it.get()); // printing the current element  

The output as expected:

    П
    р

To read the last one we have go to the end one by one. Instead let's use an extension method **LastMember** declared in **system'routines** module:

    import extensions;
    import system'routines;
 
    public program()
    {
        auto s1 := "Привет";    
        
        console.printLine(s1.LastMember);
    }

The output is

    т

#### Basic operations and functions

You can compare two strings:

    console.print("""string1"" < ""string1""=", "string1" < "string2");
    console.print("""string1"" > ""string1""=", "string1" > "string2");
    console.print("""string1"" == ""string1""=", "string1" == "string2");

with the following result:

    "string1" < "string1"=true
    "string1" > "string1"=false
    "string1" == "string1"=false

Several strings or a string and a char can be concatenated. The result of the operation is always a new string.

    import extensions;
    
    public program()
    {
        console.print("Hello" + " " + " World " + $10);
    }

The result will be

    Hello  World 

We can insert a sub string into or delete it from given string:

    console.printLine("Hillo World".delete(1,1).insert(1, "e"));

The output is:

    Hello World

The first operand is a position in the given string (*note that we may corrupt the string if a sub string will be inserted into multi-element character*). The second one is the length of the sub string for **delete** and a sub string for **insert**.

We may find a position of a sub string in the given one.

    console.printLine("Hello World".indexOf(0, " "));
    console.printLine("Hello World".indexOf(0, "!"));

The result will be

     5
     -1

The first operand is starting index and the second one is the sub string to be found. The method will return the index of the first occurrence of the second argument or -1. 

We may return a sub string from the given one:

    console.printLine("Hello World".Substring(6,5));

and the result is:

    World

Similar to the examples above the first index is a position of the sub string and the second one is its length.

The exception will be generated if the index lays outside the target string for these operations.

There is a number of extension methods for padding and trimming strings

Extension | Description
--------- | -----------
trimLeft | Removes all leading occurrences of the given character
trimRight | Removes all tailing occurrences of the given character
trim | Removes all leading and tailing occurrences of the given character
padLeft | Returns a new string of a specified length in which the beginning of the current string is padded with the given character
padRight | Returns a new string of a specified length in which the end of the current string is padded with the given character

#### Conversion to and from strings

Every ELENA class supports **Printable** property which returns a string that represents the object. By default it is a class name:

    import extensions;
    
    public program()
    {
        console.printLine(console.Printable);
    }

The output will be the full class name:

    system'$private'Console

Basic types override this method and return its text value:

    import extensions;
    
    public program()
    {
        console.printLine(12.Printable);
        console.printLine(23.4r.Printable);
    }

with the result:

    12
    23.4

Alternatively we may use **toString()** extension:

        console.printLine(12.toString());
        console.printLine(23.4r.toString());

Similar a string supports the conversion to basic types:

    int n := "12".toInt();
    real r := "23.4".toReal();

The direct conversion between a character and an integer is possible, using extensions **toInt()** and **toChar()**

    import extensions;
    
    public program()
    {
        auto ch := $78;
        int code := ch.toInt();
        
        console.printLine("code of ", ch, " is ", code);
    }

And the result will be:

    code of N is 78

### Functions

In ELENA functions are objects which handle a special message **closure** and may be declared as function literals. They are first-class functions and can be passed, stored either as weak or strong typed ones. Functions can be stateless or memory allocated (closures). A special type of functions - extension ones are supported and may operate the data without actually storing them (forming a temporal mix-in).

A function can be named or anonymous. A named one can be public or private. Anonymous (or function literal) one may encapsulate the referred variables. They can be assigned to variables and handled as normal objects.

    import extensions;
    
    F(x,y)
    {
        // returning the sum of two arguments
        ^ x + y
    }
    
    public program()
    {
        // using named private function
        console.printLine(F(1,2));
        
        // declaring and assigning anonymous function
        var f := (x,y){ ^ x * y};
        // invoking a function
        console.printLine(f(1,2));    
    }

The result is:

    3
    2

#### Returning operator

**^** operator is used to terminate the code flow and returns the given expression. The operator must be the last one in the scope.

In general, all functions (and class methods) return a value. If the value is not specified, built-in variable **self** (referring an instance of a method owner) is returned.

#### Shorthand function declaration

A function body is described inside the curly brackets. If the function contains only returning operator the body may be simplified:

    F(x,y)
       = x + y;

Anonymous functions may use **=>** operator:

        var f := (x,y => x * y);

#### Closures

Variables declared outside the function scope can be referred inside it. The appropriate boxing will be done automatically.

    import extensions;
    
    public program()
    {
        var n := 3;
        
        var f := (x => n + x );
    
        console.printLine(f(1))
    }

The result will be:

    4

#### Strong-typed arguments

The function arguments can be weak or strong-typed. 

    import extensions;
    
    Test(int x, int y)
        = x.allMask(y);
    
    public program()
    {
        console.printLine(Test(11,4));

        var test := (int x, int y => x.anyMask(y));
        console.printLine(Test(12,4));
    }
 
The result is:

    false
    true

We may use variadic arguments

    import extensions;
    
    Sum(params int[] args)
    {
        int sum := 0;
        for (int i := 0, i < args.Length, i += 1)
        {
            sum += args[i]
        };
        
        ^ sum
    }
    
    public program()
    {
        console.printLine(Sum(1));
        console.printLine(Sum(1,2));
        console.printLine(Sum(1,2,3));
        console.printLine(Sum(1,2,3,4));
    }

The result will be:

    1
    3
    6
    10
 
When an argument may be changed inside the function it should be passed with **ref** prefix:

    import extensions;
    
    Exchange(ref int l, ref int r)
    {
        int t := l;
        l := r;
        r := t;
    }
    
    public program()
    {
        int x := 2;
        int y := 3;
        
        console.printLine(x,",",y);
        
        Exchange(ref x, ref y);
        
        console.printLine(x,",",y)
    }

The result is
 
    2,3
    3,2

#### Message functions

In ELENA a message can be invoked using a special function **system'Message** which contains this message value. It can be created dynamically or using a message literal. The message literal consists of a prefix attribute **mssgconst**, the message name and the number of parameters enclosed in the square brackets:

    mssgconst writeLine[1](console,"Hello");

The result is:

    Hello

An extension message is invoked by **system'ExtensionMssage**. It contains the message value and a reference to the extension class. Similar extension literal  consists of a prefix attribute **mssgconst**, the message name, the extension reference in angle brackets and the number of parameters enclosed in the square brackets:

    var n := RealNumber.Pi / 2;
    var f := mssgconst sin<system'math'mathOp>[0];
    
    console.printLine(f(n));

The result is:

    1.0

### Control Flow

ELENA supports rich set of control flow constructs : branching, looping, exception handling. They are implemented as code-templates and can be easily extended. Branching and looping statements are controlled by boolean expressions. Though branching operator can deal with non-boolean result as well. Exceptions are used to gracefully deal with critical errors. Exceptions may contain a stack trace to identify the error source.

#### Boolean type

A **BoolValue** (alias **bool**) is a boolean type that has one of two possible values : **true** and **false**. It is a typical result of comparison operations. A boolean value is required by control flow statements (though non-boolean result can be used as well, providing a conversion operation exists). It supports typical comparison and logical operations:

    console
        .printLine("true==false : ",true == false)
        .printLine("true!=false : ",true != false)
        .printLine("true and false : ",true && false)
        .printLine("true or false : ",true || false)
        .printLine("true xor false : ",true ^^ false);

The results are:

    true==false : false
    true!=false : true
    true and false : false
    true or false : true
    true xor false : true

A boolean inversion is implemented via **Inverted** property:

    console.printLine("not ",true,"=",true.Inverted)

and the result is:

    not true=false

A boolean value can be used directly for branching operation. The message **if** with two functions without arguments can be send to it. **true** value will execute the first function, **false** - the second one.

    import extensions;
    
    public program()
    {
        var n := 2;
        
        (n == 2).if({ console.printLine("true!") },{ console.printLine("true!") });
    }

with the result:

    true!

**iif** message is supported as well: true value will return the first argument, false - the second one

    var n := 2;
    
    console.printLine((n != 2).iif("true", "false") )

The output is as expected:

    false

#### Branching operator

ELENA supports built-in branching operator **?**. It requires that an loperand is a boolean value. True-part argument follows the operator immediately. Optional false-part is introduced with colon:

    import extensions;

    public program()
    {
        var n := 2;
        
        (n == 2) ? { console.printLine("n==2") };
        (n == 3) ? { console.printLine("n==3") } : { console.printLine("n!=3") }
    }

The following result will be printed:

    n==2
    n!=3
    
If right operands are not functions the operator returns true-part if the loperand is true and false-part otherwise.

    var n := 2;
    
    console.printLine(n == 2 ? "n==2" : "n!=2");

with the similar result:

    n==2

#### Branching statements

ELENA does not reserve keywords for the control statements. But there are set of code templates that help make the code more readable. This list can be extended by programmers themselves. 

Let's start with **if-else**, known for many.

    var n := 2;
    if (n == 2)
    {
        console.printLine("n==2")
    }
    else
    {
        console.printLine("n!=2")
    }

the result is

    n==2

if we need only true-part, we may skip the else block.

    if (n == 2)
    {
        console.printLine("n==2")
    }

Alternatively only else part can be written:

    ifnot (n == 2)
    {
        console.printLine("n!=2")
    }
 
If the code brackets contains only single statement we may write the statement directly after colon:

    if(n == 2):console.printLine("n==2")
    else:console.printLine("n!=2")

#### Looping statements

ELENA provides several type of loop constructs : for, while, until, do-until, do-while.

**FOR** loops is used to execute the loop body for the specified iteration. It consists of initialization, condition and iteration expressions separated by commas and the main loop body:

    import extensions;
    
    public program()
    {
        for (var i := 0, i < 5, i += 1)
        {
            console.printLine(i)
        }
    }

The first expression declares and initializes the loop variable, the second one is checked if the variable is inside the iteration range and the third one is increased the loop variable after the main loop body is executed.

The result will be:

    0
    1
    2
    3
    4

If we need to execute an iteration step before the condition check we may skip the last expression. In this case the first expression will both initialize and iterate the loop variable on every step. The condition (second expression) will work similar, the loop continues until a false value is encountered. As a result the iteration step is guarantee to be executed at least once.

    import extensions;
    
    public program()
    {
        var sum := 0;
        for(var line := console.readLine(), line != emptyString)
        {
            sum += line.toInt()
        };
        
        console.printLine("The sum is ", sum)
    }

In this example we read numbers from the console until an empty line is encountered, and print the sum. A method **readLine** reads the next line of characters from the console. **emptyString** is an empty string literal constant. An extension method **toInt** converts a string to an integer.

The output may look like this:

    1
    3
    5
    
    The sum is 9

**WHILE** is a classical loop construct, it executes the code while the condition is true. Let's write the enumeration loop. **Enumerators** are special objects which enumerates collections, returning their members one after another. We will enumerate a string:

    import extensions;
    
    public program()
    {
        auto s := "♥♦♣♠";
        auto e := s.enumerator();
        while (e.next()) {
            console.printLine(e.get())
        }
    }

The result will be:

    ♥
    ♦
    ♣
    ♠

The opposite construct is **UNTIL**. It repeats the code until the condition is true. Let's repeat the code while the number is not zero:

    import extensions;
    
    public program()
    {
        var n := 23;
        until(n == 0)
        {
           console.printLine(n);
         
           n /= 3
        }
    }

The output will be:

    23
    7
    2

It is clear that both loop constructs are interchangeable. The choice may depend on rather semantic meanings : to underscore one condition over another one.

If we need to execute the loop at least once whatever condition is, we have to use **DO-WHILE** or **DO-UNTIL**. Let's calculate the factorial of 5:

    import extensions;
    
    public program()
    {
        int counter := 5;
        int factorial := 1;
    
        do {
            factorial *= counter;
            counter -= 1
        }
        while:(counter > 0);
        
        console.printLine(factorial)
    }

*Note that we have to put colon after **while** token*

The result as expected:

        120
 
#### Exception handling

Exception handling is designed to deal with unexpected situations during the program executing. For dynamic language the typical unexpected situation is sending a message to the object which does not handle it (no appropriate method is declared). In this case **MethodNotFound** exception is raised. Another notable examples are : dividing by zero, nil reference and out of memory exceptions. The language provides several code templates to deal with such situations.

Typical exception in ELENA inherits **system'Exception** base class. A method named **raise()** is declared and used to raise an exception in the code. There are several exceptions in **system** library :  OutOfRangeException, InvalidArgumentException, InvalidOperationException, MethodNotFoundException, NotSupportedException, AbortException, CriticalException and so on. A typical exception contains the error message and a call stack referring to the moment when the exception was created. 

Raising an exception in ELENA is straightforward.

    divide(l,r)
    {
        if(r == 0)
        {
            InvalidArgumentException.raise()
        }
        else
        {
            ^ l / r
        }
    }

To handle it we have to write **TRY-CATCH** or **TRY-FINALLY** constructs.

    public program()
    {
        console.print("Enter the first number:");
        var l := console.readLine().toInt();
        
        console.print("Enter the second number:");
        var r := console.readLine().toInt();
    
        try
        {
            console.printLine("The result is ", divide(l,r))
        }
        catch:(InvalidArgumentException e)
        {
            console.printLine("The second argument cannot be a zero")
        };
    }

The output will be:

    Enter the first number:2
    Enter the second number:0
    The second argument cannot be a zero

An anonymous function declared after **catch** token is invoked if the raised exception matches the function argument list. If we want to handle any standard exception we may use a base exception class:

        catch:(Exception e)
        {
            console.printLine("An operation error")
        };

Several exception handlers may be declared:

    catch:(InvalidArgumentException e)
    {
        console.printLine("The second argument cannot be a zero")
    }
    catch:(Exception e)
    {
        console.printLine("An operation error")
    };

In our case both handlers are matched to InvalidArgumentException exception (because InvalidArgumentException is a child of Exception) but the proper exception handler will be raised because it is described early.

If we need to execute the code whenever an exception is raised or the code works correctly we have to use **TRY-FINALLY-CATCH** construct. The typical use-case is a resource freeing.

    import extensions;
    
    public program()
    {
        console.printLine("Try");
        try
        {
            var o := new Object();
            o.fail();
        }
        finally
        {
            console.printLine("Finally");
        }
        catch:(Exception e)
        {
            console.printLine("Error!");
        }
    }

The output will be:

    Try
    Finally
    Error!

If the second line is commented out the output will be:

    Try
    Finally

If we want to execute the code after the operation whenever an exception is raised, we may skip catch part:

        console.printLine("Try");
        try
        {
            var o := new Object();
            o.fail();
        }
        finally
        {
            console.printLine("Finally");
        }

And the result is:

    Try
    Finally
    system'Object : Method fail[0] not found
    Call stack:
    system'Exception#class.new[1]:exceptions.l(96)
    system'MethodNotFoundException#class.new[1]:exceptions.l(1)
    system'MethodNotFoundException#class.new[2]:exceptions.l(190)
    mytest'program.#invoke[0]:test.l(14)
    mytest'program.#invoke[0]:test.l(9)
    system'$private'entry.#invoke[0]:win32_app.l(37)
    system'#startUp:win32_app.l(55)

### Symbols

Symbols are named expression which could be use to make the code more readable. A typical symbol is a constant declaration. Though they can more. Symbols are used to declare global singleton instances. The module initialization code is implemented via them. They can be private or public.

#### Values / Constants 

A typical symbol use case is a value which can be reused further in the code:

    import extensions;
    import extensions'math;
    
    real PiOver2 = RealNumber.Pi / 2;
    
    public program()
    {
        console.printLine("sin(",PiOver2,")=", sin(PiOver2))
    }

Here we declare a private strong-typed symbol PiOver2. The type may be omitted. In this case it will be a weak typed value. The result is:

    sin(1.570796326795)=1.0

If the symbol should be public (be accessible outside the module) **public** attribute should be put before the type and the name.

If the symbol value can be calculated in compile-time we may declare a constant symbol

    import extensions;
    import extensions'math;
    
    public const int N = 3;
    
    public program()
    {
        for (int i := 0, i < N, i += 1)
        {
            console.printLine(i,"^2=", sqr(i))
        }
    }

The output is:

    0^2=0
    1^2=1
    2^2=4

#### Static symbols

A normal symbols is evaluated every time it is used. A static symbol is a special case which is initialized only once by the first call (so called lazy loading) and its value is reused for every next call. It is a preferred way to implement a singleton (if it is not stateless).

    import extensions;
    import extensions'math;
    
    static inputedNumber = console.print("Enter the number:").readLine().toInt();
    
    public program()
    {
        console.printLine(inputedNumber,"^3 = ", power(inputedNumber, 3))
    }

In this example the symbol is evaluated only once and the entered value is preserved, so the number should be entered only once:

    Enter the number:4
    4^3 = 64

#### Preloaded symbols

A preloaded symbol is a special symbol which is automatically evaluated on the program start if namespace members are used in the program. So it may be used for module initialization.

    import extensions;
    
    preloaded onModuleUse = console.printLine("Starting");
    
    public program()
    {
        console.printLine("Hello World")
    }

The output is:

    Starting
    Hello World

### Types

In programming a data type (or simply type) defines the role of data : a number, a text and so on. In object-oriented this concept was extended with a class which encapsulates the data and operations with them. The data is stored in fields and the operations are done via methods. In ELENA both a type and a class means mostly the same and can be used interchangeably without loss of meaning (except primitive types). Classes form a hierarchy based on inheritance. It means every class (except a super one - **system'Object**) has a parent and a single one.

A programming language can have static or dynamic type systems. In OOP classes can be made polymorphic fixing part of the problem with static types. Dynamic languages resolve types in run-time. This affects their performance. Modern programming languages as a result of it try to combine these approaches. In ELENA the types are dynamic and are resolved in run-time. In this sense you may write your program without explicitly specifying types at all. But in most cases the types should be specified to make your code more readable and to improve it performance.

In dynamic languages the operation to invoke a method is usually called sending a message. The class reacts to the message by calling appropriate method (with the same name and a signature). In normal case it is done via searching the appropriate handler in the method table. And it takes time. To deal with this ELENA allows to declare **sealed** or **closed** classes. Methods of sealed class can be resolved in compile-time and called directly. Methods of closed classes (or interfaces) can be resolved in compile-time via a method table. But the sealed class can not be inherited. Interfaces can be inherited but no new methods can be declared (except private ones).

In ELENA typecasting are implemented via a sending a special methods - conversion handlers. Every class may be converted into another one if it handles this message. Otherwise an exception is raised.

#### Primitive types

In most cases a class and a type mean the same. The only exception is a primitive type. Primitive types are built-in and support only predefined operations. They are classical data types in sense that they are pure data. The following types are supported by ELENA:

Type | Size | Description
---- | ---- | -----------
__float | 8 | A 64-bit floating-point number
__int | 1 | A 8-bit integer number
__int | 2 | A 16-bit integer number
__int | 4 | A 32-bit integer number
__int | 8 | A 64-bit integer number
__raw |  | a raw data
__ptr | 4 | a 32-bit pointer
__mssg | 4 | a message reference
__mssg | 8 | an extension reference
__subj | 4 | a message name reference
__symbol | 4 | a symbol reference
__string | | an array

Primitive types can be used in the program with a help of appropriate wrappers. Every time a primitive type is used in the code (except primitive operations) it is boxed into its wrapper. After the operation it is unboxed back. Due to performance issues no validation is applied to the primitive operations, so it is up to a programmer (or a wrapper class) to handle it correctly. 

#### Classes

Putting aside primitive types every program object is an instance of a class. A class is a structure encapsulating data (fields) with operations with them (methods). A class may specify constructors to create an object and conversion routines to convert it from another object.

Declaring and using classes is straightforward for anyone familiar with C-like object-oriented languages:

    import extensions;
    
    // declaring a class named A
    class A
    {
        // declaring a field named a
        field a;
        
        // declaring an implicit constructor
        constructor()
        {
            a := "Some value"
        }
        
        // declaring a method named printMe
        method printMe()
        {
            console.printLine(a)
        }
    }
    
    public program()
    {
        // creating an instance of A class
        var o := new A();
        
        // calling a method 
        o.printMe()
    }

The output is:

    Some value

The keyword **class** specifies a normal class. A class body is enclosed in curly brackets. A class name declared right before the class body. A field may be declared in any place inside the class body. It may starts with an attribute **field**. Implicit constructor is named **constructor**. A method may be declared with a keyword **method**. The code could be placed inside curly brackets.

The classes form an hierarchy. Every one (except the super one) has a parent. If the parent is not specified the class inherits **system'Object** (a super class). A child class can override any non-sealed method of its parent. An instance of a child class is simultaneously of a parent type (so called is-a relation). So we could assign a child to a variable of a parent type. But the overridden methods will be used (polymorphic code). In case of weak types (or type-less) this is true as well (we only assume that all variables are of the super type).

    import  extensions;
    
    class Parent
    {
        field f;
        
        constructor()
        {
            f := "some value"
        }
        
        printMe()
        {
            console.printLine("parent:",f)
        }
    }
    
    class Child : Parent
    {
        printMe()
        {
            console.printLine("child:",f)
        }
    }
    
    public program()
    {
        Parent p := new Parent();
        Child c := new Child();
        
        p.printMe();
        c.printMe()
    }

The output will be:

    parent:some value
    child:some value

The parent clause should follow the class name and be introduced with a colon. To override the method we have only declare it once again with the same name and signature.

In ELENA a type (or a class) can be used directly in the code like any other symbols. The only difference that we cannot invoke implicit constructor. The explicit one should be used instead.

    import extensions;
    
    // declaring a class named A 
    class A
    {
        field a;
        
        // implicit constructor
        constructor()
        {
            a := "a"
        }
        
        // explicit constructor named new
        constructor new()
            // invoking an implicit constructor
            <= ();
    }
    
    // declaring a class named B
    class B
    {
        field b;
        
        // implicit constructor
        constructor()
        {
            b := 2
        }
        
        // explicit constructor named new
        constructor new()
            // invoking an implicit constructor
            <= ();    
    }
    
    // factory function
    factory(class)
        // sending a message - name and returning a result of the operation 
        = class.new();
    
    public program()
    {
        // creating objects using a class directly
        var a := factory(A);
        var b := factory(B);
        
        // printing the object types
        console
            .printLine(a)
            .printLine(b);
    }

The output is:

    mylib'$private'A
    mylib'$private'B

By default a class is declared private. It means it cannot be accessed outside its namespace. In the case of a library we would like to reuse it. So we have to provide a public attribute:

    public class B
    {
    }

Now the class can be accessed either outside the library or by a reference:

    public program()
    {
        var b := new mylib'B()
    }

A reference (or a full name) consists of a namespace (which in turn may contain sub-namespaces separated by apostrophes) and a proper name separated by an apostrophe.

#### Abstract classes

An abstract class is a special class used as a basis for creating specific classes that conform to its protocol, or the set of operations it supports. Abstract classes are not instantiated directly. It differs from an interface that the children can extend its functionality (declaring new methods). The abstract class may contain both abstract and normal methods. An abstract method must have an empty body (opening and closing curly brackets)

    import extensions;
    
    abstract class Bike
    {  
        abstract run() {}  
    }
      
    class Honda4 : Bike
    {  
        // overriding an abstract method
        run()
        {
            console.printLine("running safely")
        }
    }    
    
    public program()
    {
        Bike obj := new Honda4();
        obj.run()
    }        

The output will be:

    running safely

Classes based on an abstract class must implement all the parent abstract methods. If a child class adds a new abstract methods or implements only some of the parent abstract methods, it should be declared abstract as well.

    abstract class BikeWithTrolley : Bike
    {
        abstract pack() {}
    }

#### Interfaces

An interface is a special case of an abstract class. For performance reason a new method cannot be declared for the interface children (except private ones). As a result an interface method can be called semi-directly (via a method table). An interface methods can be both abstract and normal ones (like in abstract classes).

    interface IObserver
    {
        // declaring an interface abstract method
        abstract notify() {} 
    }

Though we could directly inherit the interface:

    class Oberver : IObserver
    {
        notify() 
        {
            console.printLine("I'm notified")
        }
    }

it is unpractical, because we cannot extends the class functionality. Instead we may use a template named **interface**:

    import extensions;
    
    // declaring an interface
    interface IObserver
    {
        // declaring an inteeface method to be implemented
        abstract notify() {}
    }
    
    // creating a class supporting an interface
    class MyClass : interface<IObserver>
    {
        // implementing an interface method
        notify() 
        {
            console.printLine("I'm notified")
        }
        
        // implementing some other functionality
        doSomework()
        {
            console.printLine("I'm doing some work")
        }
    }
    
    // simply routine to invoke the interface
    sendNotification(IObserver observer)
    {
        observer.notify()
    }
    
    public program()
    {
        // creating an intance of MyClass
        auto myObject := new MyClass();
        
        // do some work
        myObject.doSomework();
        
        // passing the interface implementation to the function by 
        // explicit typecasting the object
        sendNotification(cast IObserver(myObject));    
    }        

The output is:

    I'm doing some work
    I'm notified

The class may implement several interfaces (so we may by-pass a problem with a single inheritance). 

    import extensions;
    
    // declaring an interface
    interface IObserver
    {
        // declaring an inteeface method to be implemented
        abstract notify() {}
    }
    
    // declaring the second interface
    interface IWork
    {
        abstract doSomework() {}
    }
    
    // creating a class supporting both interfaces
    class MyClass : interface<IObserver>, interface<IWork>
    {
        // implementing an interface method
        notify() 
        {
            console.printLine("I'm notified")
        }
        
        // implementing the second interface
        doSomework()
        {
            console.printLine("I'm doing some work")
        }
    }
    
    // simply routine to invoke the interface
    sendNotification(IObserver observer)
    {
        observer.notify()
    }
    
    public program()
    {
        // creating an intance of MyClass 
        auto myObject := new MyClass();
        
        // implicitly typecast it to the second interface
        IWork work := myObject;
        
        // use the second interface
        work.doSomework();
        
        // passing the interface implementation to the function by 
        // explicit typecasting the object
        sendNotification(cast IObserver(myObject));    
    }

with the same result:

    I'm doing some work
    I'm notified
        
#### Singletons

Singletons are special classes which should no be initialized (constructors are not allowed) and only a single instance exists. Singletons are mostly stateless (though they may have static fields). They are always sealed.

The singleton class is declared with **singleton** attribute. It can be used directly using a class reference:

    import extensions;
    
    // declaring a singleton
    public singleton StringHelpers
    {
        // Gets the first character of a string.
        char first(string str)
        {
            ^ str[0]
        }
    }
    
    public program()
    {
        var str := "My string";
        
        // calling a singleton method
        console.printLine("Calling StringHelpers.First(""",str,""")=", 
             StringHelpers.first(str))
    }

The output will be:

    Calling StringHelpers.First("My string")=M

#### Structs

Structs are special kind of classes which are stored in-place, rather than by reference in the memory heap. Structs are sealed (meaning they cannot be inherited). They hold mostly small data values (such as numbers, handlers, pointers). All primitive data handlers are structs.

Struct fields can be either primitive or another structures. No reference types are allowed. All fields should be strong typed for that reason.

In most cases the use of structs is quite straightforward.

    import extensions;
    
    // declaring a struct
    struct Record
    {
       // declaring struct fields
       int x;
       int y;
       
       // declaring struct constructor
       constructor(int x, int y)
       {
           // using this prefix to distinguish a class member from the local one
           this x := x;
           this y := y
       }
       
       printMe()
       {
           console.printLine("Record(",x,",",y,")");
       }
    }    
    
    public program()
    {
        // creating a struct
        auto r := new Record(2, 4);
        
        // invoking struct method
        r.printMe()
    }

The result will be:

    Record(2,4)

Note that structs are stored in-place. It means that in our example the object was declared in the method stack. Every time it is used as a weak type, it will be boxed and unboxed after the operation. To improve performance we can declare a struct to be a constant one to avoid unboxing operation. For example all numbers are constant structs:

    // declaring a constant struct
    public const struct IntNumber : IntBaseNumber
    {
        // a field is a primitive 32-bit integer type   
        embeddable __int theValue[4];
    ...
 
#### Strings

Strings are special type of classes (both structs and nonstructural classes) which is used to contain the arrays. As a result the class length is variable. No default constructors (without parameters) are allowed.

    public const struct String : BaseValue
    {
        __string byte[] theArray;
    
        constructor allocate(int size)
            = new byte[](size + 1);

It may contain only a single field markred as **__string** one.

#### Extensions

Extensions are special stateless classes used to declare extension methods. Extension methods allow you to extend the original class functionality without creating a new derived type, recompiling, or otherwise modifying the original type. Every method declared in an extension class is an extension method.

In normal cases extension classes is never used directly (except using in mixins). To be used in other modules they should be declared as a public one. To start to use the extension it is enough to declare it in the same namespace as the code where it will be used. If the extension is declared in another module, it should be included into the code (using **import** statement).

    import extensions;
    
    // declaring an extension
    public extension MyExtension
    {
        // every member of the extension is an extension method
        printMe()
        {
            console.printLine("I'm printing ", self);
        }
    }
    
    public program()
    {
        // invoking an extension method for various types
        2.printMe();
        "abc".printMe();
        2.3r.printMe()
    }

The result is:

    I'm printing 2
    I'm printing abc
    I'm printing 2.3

Extensions methods are used in ELENA in many places. For example **print** and **printLine** are extension variadic methods. If the class already have the method with the same name it will be used instead of extension one. For example if we extend our previous example with a new class containing the method **printMe**

    MyClass
    {
        printMe()
        {
            console.printLine("I'm printing myself");
        }
    }
    
    public program()
    {
        auto o := new MyClass();
        o.printMe();
    }

The correct method will be invoked:

    I'm printing myself

But if the object is of weak type, the extension will be called:

    public program()
    {
        var o := new MyClass();
        o.printMe();
    }

The output will be:

    I'm printing mytest'$private'MyClass

So it is a good practice not to mix the extension and normal method names.

Extensions can be weak one, meaning that they can extends any object (or instances of **system'Object** class). But we could always specify the exact extension target:

    // declaring an extension of MyClass
    public extension MyStrongExtension : MyClass
    {
        printMe()
        {
            console.printLine("I'm printing MyClass");
        }
    }
    
    MyClass
    {
    }
    
    public program()
    {
        auto o := new MyClass();
        o.printMe();
    }

The output will be:

    I'm printing MyClass

It is possible to have several extension method with the same name as long as the extension targets are not the same.
   
#### Sealed / Closed Classes

**Sealed** and **closed** attributes are used to improve the performance of the operations with the classes. A sealed class cannot be inherited. As a result the compiler may generate direct method calls for this class (of course when the type is known). All structs and singletons are sealed by default. Declaring a sealed class is quite simple:

    sealed class MySealedClass
    {
    }

Closed classes can be inherited but no new methods (private ones or new fields are allowed). All interfaces are closed. When the type is closed, the compiler may use a method table to resolve the method call.

    closed class MyBaseClass
    {
        // declaring a "virtual" method
        myMethod() {}
    }
    
    class MyChileClass : MyBaseClass
    {
        // overriding a method
        myMethod() {}
    }

### Fields

A field is a variable that is declared in a class or struct. Fields are class members and used to hold its state. Fields can be weak (or of system'Object type) or strong ones. Struct fields may be primitive or another structs. They cannot be weak. Both fixed size and dynamic arrays are supported. All fields are private ones. They cannot be accessed outside its class.

#### Class Fields

A class field should be declared inside the class body. **Field** and type attributes are optional.

    class MyClass
    {
        // a fully qualified field declaration
        field int x;
        
        // field attribute my be omitted
        int y;
        
        // type attribute my be omitted
        field z;
        
        // the minimal declaration
        f;
    }

The fields can be accessed in the method by their name.

    import extensions;
    
    class MyClass
    {
        // declaring a field named x
        int x;
    
        constructor()
        {
            // constructos are typical places to initialize fields
            x := 2
        }
    
        printX()
        {
            // field is referred by its name
            console.printLine("MyClass.x="x)
        }    
    }    
    
    public program()
    {
        var o := new MyClass();
        
        o.printX()
    }

The output will be:

    MyClass.x=2

If a local variable and a field names are the same we may use **this** prefix to tell a field apart:

    class MyClass
    {
        // declaring a field named x
        int x;
    
        // declaring an argument named x
        constructor(int x)
        {
            // to tell apart a field from a local, this qualifier should be used
            this x := x
        }
    
        printX()
        {
            // field is directly referred using this prefix
            console.printLine("MyClass.x=",this x)
        }    
    }
    
    public program()
    {
        var o := new MyClass(3);
        
        o.printX()
    }

The output will be:

    MyClass.x=3

#### Field Initializer

Constructors are traditional place to initialize the fields. But in some cases it is more convenient to do it in place, after the declaration.  

    class Record
    {
        // an initialization expression can be a constant
        int x := 0;

        // it can be a result of operations  
        string y := "My" + "String";

        // or class creation  
        z := new Object(); 
    }

In this case, we could not declare a constructor at all and use the default one:

    public program()
    {
        var p := new Record();
    }

An initialization expression can be used alone, for example in the derived classes. In this case we have to use **this** qualifier:

    class MySpecialRecord : Record
    {
        // we have to use this prefix to initialize already existing fields
        this x := 2; 
    } 

And of course we could use it for initializing class fields right in the code:

    struct Point
    {
        int x := 0;
        int y := 0;
        
        printMe()
        {
            console.printLine("x:",x,",y:",y)
        }
    }
    
    public program()
    {
        var p := new Point::
        {
            this x := 1;
            
            this y := 2; 
        };
        
        p.printMe()
    }

And the output is:

    x:1,y:2

#### Constant fields

Constant fields are special type of class fields which can be initialized only in class constructors.

    class MyClass
    {
        // declaring a contant field
        const int x;
        
        constructor(int x)
        {
            // have to be initialized in the constructor
            this x := x
        }
        
        printMe()
        {
            // can be used in class methods but cannot be changed
            console.printLine("MyClass.x=",x)
        }
    }

#### Static fields

Static fields are class variables which are shared by all class instances. They can be accessed both from normal and static methods. A value changed in one class instances, will be changed for all class instances. Static fields are inherited by the children classes. But in this case the changes are visible only per class. For example in the following code:

    abstract class Base
    {
        // declaring a static field
        static nameField;
        
        // declaring a method to access a static field
        printName()
        {
            console.printLine(nameField)
        }
    }
    
    // inheriting the base class
    class A : Base
    {
        // setting a value for the class A
        this nameField := "A";        
    }
    
    class B : Base
    {
        // setting a value for the class B
        this nameField := "B";
    }
    
    public program()
    {
        var a := new A();
        var b := new B();
        
        a.printName();
        b.printName();
    }

The output will be:

    A
    B

So the value of a static field changed for the class A is visible only in instances of A and similar for the class B.

Note that singleton my have static fields as well:

    singleton MySingleton
    {
        static mySingletonField := "Some value";
        
        printMe()
        {
            console.printLine(mySingletonField)
        }
    }
    
    public program()
    {
        MySingleton.printMe()
    }

And the result should be:

    Some value

#### Static constant fields

Static fields can be constant. It means they have to be initialized right after the declaration and cannot be changed in the code.

    class A
    {
        // declaring and intializing a constant static field
        const static classNameField := "A";
        
        // declaring a method to access a static field
        printName()
        {
            console.printLine(classNameField )
        }
    }

It can be inherited as well and should be initialized for every derived class.

#### Static sealed fields

In case if we do not like to inherit static fields, we have to declare them sealed. In this case they have to be initialized only once.

    abstract class Base
    {
        // declaring a static field
        sealed static nameField := "Hello from Base";
        
        // declaring a method to access a static field
        printName()
        {
            console.printLine(nameField)
        }
    }    
    
    // inheriting the base class
    class A : Base
    {
        constructor() {}
    }
    
    class B : Base
    {
        constructor() {}
    }
    
    public program()
    {
        var a := new A();
        var b := new B();
        
        a.printName();
        b.printName();
    }

And the result is:

    Hello from Base
    Hello from Base

As you see sealed static fields are similar to the traditional static fields in the languages like C#.

#### Static attribute fields

Static attribute fields are special case of static fields which value are inherited by the children. They can be used to provide class meta information. For example using so-called accumulator attributes we may extend the class meta information in the derived classes:

    import extensions;
    
    class Base
    {
        // declaring accumulator attribute
        attribute object[] theFamily;
        
        // adding a reference to the class
        this theFamily += Base;
        
        static printFamily()
        {
            console.printLine(theFamily.asEnumerable())
        }
    }
    
    class A : Base
    {
        // adding a reference to the derived class
        this theFamily += A;
    }    
    
    class B : Base
    {
        // adding a reference to the derived class
        this theFamily += B;
    }    
    
    public program()
    {
        console.print("A:");
        A.printFamily();
        
        console.print("B:");
        B.printFamily();
    }

The result is:

    A:mytest'$private'Base#class,mytest'$private'A#class
    B:mytest'$private'Base#class,mytest'$private'B#class

An accumulator attribute of the class A contains the class and its parent. The similar attribute of the class B contains its own copy of the attribute. If a new class will be derived from B, the attribute will be inherited and can be extended further and so on.

#### Primitive fields

Primitive types are boxed into corresponding wrapper structures. To declare such a structure we have to declare a primitive field:

    // declaring an 32bit integer wrapper
    struct MyIntNumber
    {
        embeddable __int theValue[4];
    } 
    
    // declaring an Unicode character wrapper
    struct MyCharValue
    {
        embeddable __raw theValue[4];
    }
    
    // declaring a 64bit floating-point number wrapper
    struct MyRealNumber
    {
        embeddable __float theValue[8];
    }

#### Fixed size array field

It is possible to declare fixed size array in a data structure. They are useful when you write methods that interop with data sources from other languages or platforms. The array type must be a structure itself. To declare a fixed size array field the field name should be followed by the size constant enclosed in square brackets:

    struct WSADATA
    {
        short wVersion;
        short wHighVersion;
        // declaring a fixed size arrays of bytes 
        byte  szDescription[257];
        byte  szSystemStatus[129];
    }

#### Dynamic size array field

Arrays are special case of primitive built-in types. The array declaration consists of the element type and empty square brackets (**\[\]**). A primitive arrays are encapsulated into a string classes (both reference and structure ones). The string class must have only one field of an array type with **__string** prefix.

    class MyArray
    {
        // declaring an array of system'Object  
        __string object[] theArray;
    
        constructor allocate(int len)
            = new object[](len);
    }
    
    struct MyString
    {
        // declaring an array of system'CharValue
        __string char[] theArray;
        
        constructor allocate(int len)
            = new char[](len);
    }

### Methods
