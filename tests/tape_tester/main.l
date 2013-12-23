#define system.
#define system'dynamic.
#define extensions'dynamic.

// --- Interpreter ---

#class BFTape
{
    #field theArray.
    #field thePointer.

    #constructor new &type'length:aLength
    [
        thePointer := Integer new:0.

        theArray := Array new &type'length:aLength &function: &&:n [ Integer new:0 ].
    ]

    #method type'tape = $self.

    #method append
    [
         (theArray@thePointer) += 1.
    ]
    
    #method reduce
    [
         (theArray@thePointer) -= 1.
    ]
    
    #method next
    [
        thePointer += 1.
    ]

    #method previous
    [
        thePointer -= 1.
    ]
    
    #method input
    [
        (theArray@thePointer) write &type'int:(console readChar).
    ]
    
    #method output
    [
        console write:(CharValue new:(theArray@thePointer)).
    ]
    
    #method get = theArray@thePointer.
}

#symbol InterpreterTest =
[
//    console readChar.
    
    #var l := scriptControl load &path:"D:\Alex\ELENA\tests\tape_tester\helloworld.es".
].

//// === Roles ===
//
//#class(role)EGetProp
//{
//}
//
//#symbol GetPropertyTester =
//[
//    #var msg := %Length.
//    #var prop := %Length(3).
//    
//    #var l := prop~msg get.
//    
//    #var aTape := Struct(%X, 2, %Y, 3).
//    #var x := aTape X.
//    #var y := aTape Y.
//].

#symbol ReflectionTester =
[
    #var anObject := String new:"abd".

    #var aPresentation := anObject~ETextSerializer Serialized.
].

#symbol TapeTest =
[
   #var aTape := Tape(
       "Hello World", 
       system'console,
       %(eval&writeLine),
       system'console,
       %(readChar)).

    aTape eval.
].

//#symbol ClassTest =
//[
//    #var n := Integer new.
//
//    #var g := Class(%(get&next(0)), Tape(%(get&index), 1, tapeControl, %(get(2)), %(append(1))), %(get&index), [ n ]).
//
//    g next.
//].

#symbol Program =
[  
//    ClassTest eval.
//    GetPropertyTester eval.

//    ReflectionTester eval.
//    TapeTest eval.
    InterpreterTest eval.
].