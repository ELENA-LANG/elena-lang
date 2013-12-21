#define system.
#define system'dynamic.
#define extensions'dynamic.

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

#class Interpreter
{
    #field theIndex.
    #field theValue.
    
    #constructor new
    [
        theIndex := Integer new.
        theValue := Integer new.
    ] 
    
    #method next
    [
        theIndex += 1.
    ]
    
    #method inc
    [
        theValue += 1.
    ]
}

#symbol TapeTest2 =
[
    #var aTape := Tape(
        1, tapeControl,
        %(getAt(15)),
        %next,
        %next,
        %(get&inc(0)),
        %(get&inc(0))).
        
    #var retVal := aTape eval:(Interpreter new).
        
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
//
//    GetPropertyTester eval.

//    ReflectionTester eval.
//    TapeTest eval.
    TapeTest2 eval.
].