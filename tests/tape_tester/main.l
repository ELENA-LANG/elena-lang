#define system.
#define system'routines.
#define system'dynamic.
#define extensions'dynamic.

//// --- Interpreter ---
//
//#class BFTape
//{
//    #field theArray.
//    #field thePointer. 
//
//    #constructor new &length:aLength
//    [
//        thePointer := Integer new:0.
//
//        theArray := arrayControl new &length:aLength &each: n [ Integer new:0 ].
//    ]
//
//    #method tape = $self.
//
//    #method append
//    [
//         (theArray@thePointer) += 1.
//    ]
//    
//    #method reduce
//    [
//         (theArray@thePointer) -= 1.
//    ]
//    
//    #method next
//    [
//        thePointer += 1.
//    ]
//
//    #method previous
//    [
//        thePointer -= 1.
//    ]
//    
//    #method input
//    [
//        (theArray@thePointer) write &int::(console readChar).
//    ]
//    
//    #method output
//    [
//        console write:(CharValue new:(theArray@thePointer)).
//    ]
//
//    #method run : aLoop
//    [
//        control while:[ 0 < (theArray@thePointer) ] &do: [ aLoop eval:self ].
//    ]
//            
//    #method get = theArray@thePointer.
//}
//
//#symbol InterpreterTest =
//[
//    //console readChar.
//    
//    #var l := scriptControl load &path:"d:\alex\elena\tests\tape_tester\sample.es".
//
//    #var aTape := BFTape new &length:100.
//
//    l eval:aTape.
//].
//
//// === Roles ===
//
//#class(role)EGetProp
//{
//}
//
//#symbol GetPropertyTester =
//[
//    #var subj := %length.
//    #var prop := %length [ 3 ].
//    
//    #var l := prop~subj get.
//
//    #var aTape := Struct(%X, 2, %Y, 3).
//    #var x := aTape X.
//    #var y := aTape Y.
//].  
//
//#symbol ReflectionTester =
//[
//    #var anObject := String new:"abd".
//
//    #var aPresentation := anObject~textserializable literal. 
//    
//    #var aClass := Symbol new &literal:"system'Integer".
//    #var anInstance := aClass new.
//].

#symbol TapeTest =
[
   #var aTape := Tape(
       1, 
       system'dynamic'tapeControl,
       %read(1,),
       system'console,
       %writeLine(1),
       system'console,
       %readChar(0)).

    aTape eval:"Hello World".
].

//#symbol ClassTest =
//[
//    #var n := Integer new.
//
//    #var g := system'dynamic'Class(%next(0), system'dynamic'Tape(1, 1, system'dynamic'tapeControl, %read(1,), %index(0), %append(1)), %index(0), [ n ]).
//
//    g next.
//].

#symbol Program =
[ 
//    InterpreterTest eval.

//    ClassTest eval.
//    GetPropertyTester eval.
//    ReflectionTester eval.
    TapeTest eval.
].