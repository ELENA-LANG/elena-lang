////#import system'routines.
////#import system'collections.
//#import system'dynamic.
////#import extensions'text.
import extensions'scripting.

InterpreterTest =
[
    scriptEngine 
        load path:"~\scripts\assembly.es";
        load path:"~\scripts\eson.es".

//    var s := scriptEngine eval:"{ x : 1 , y : 2 , writeLine : function(x){ system'console.writeLine(x) } }".
    var s := scriptEngine eval:"#import ""system'console"" as console ; program={ writeLine : function(x){ console.writeLine(x);}; };".

    var lab := Library new.
    var obj := s eval:lab; first.

    obj writeLine("Script: function works").
    
//    #var x := s x.
//    #var y := s y.
//    
//    ((x == 1)and:(y == 2))?
//    [
//        console writeLine:"Script: structure works".
//    ].
//    
//    s writeLine:"Script: function works".
].

////#symbol ReflectionTester =
////[
////    #var s := 2 toTextSerialized. 
////        
////    #var anObject := String new:"abd".
////
////    #var aPresentation := anObject toTextSerialized. 
////].
//
//#symbol ClassTest =
//[
//    #var n := Integer new.
//  
//    #var g := Struct(%getNext, Tape(1, 1, %"tapeOp.var[]", %"get&index", %"append[1]"), %index, n).        
//
//    g eval &getNext.
//].
//
//#symbol SubjectSerializerTest =
//[
//    #var subj := %length.
//    
//    #var subjName := %length literal.
//    
//    #var newSubj := Signature new &literal:subjName.
//    
//    (subj equal &subject:newSubj)
//        ? [ console writeLine:"Subject serializing works". ]
//        ! [ console writeLine:"Subject serializing does not work". ].
//].
//
//#symbol ClassSerializerTest =
//[
//    #var aClass := Symbol new &literal:"system'IntNumber".
//    #var anInstance := aClass new.
//    
//    #var className := anInstance className.
//    
//    (className == "system'IntNumber")
//        ? [ console writeLine:"Class serializing works". ]
//        ! [ console writeLine:"Class serializing does not work". ].    
//].
//
////#class A
////{
////    #method a
////    [
////        console writeLine:"A.a called".
////    ]
////}
////
////#class B
////{
////    #method b
////    [
////        console writeLine:"B.b called".
////    ]
////}
////
////#class C
////{
////    #field theObject.
////    
////    #constructor new : object
////    [
////        theObject := object.
////    ]
////
////    #method a
////    [
////        console writeLine:"C.a called".
////        
////        theObject a.
////    ]
////
////    #method b
////    [
////        console writeLine:"C.b called".
////        
////        theObject b.
////    ]
////                
////    #method c
////    [
////        console writeLine:"C.c called".
////    ]
////    
////    #method => theObject.
////}
////
////#class(extension)configOp
////{
////    #method loadConfiguration
////    [
////        #var stack := Stack new push:$nil.
////        #var counter := Integer new.
////        
////        TokenEnumerator new &literal:(self literal) &token_dfa:(ExpressionStatemachine new) run &each:token
////        [
////            token =>
////                "(" ? [ counter *= 16. ]
////                ")" ? [
////                    #var(int) argCounter := counter int mod:16.
////                    counter /= 16.
////                    
////                    #var arg := $nil.
////                    #loop (argCounter > 0)?
////                    [
////                        arg := stack pop group &with:arg.
////                        
////                        argCounter -= 1.
////                    ].
////                    
////                    #var target := stack pop.
////                    ($nil != target)?
////                    [
////                        ($nil == arg)
////                            ? [ stack push:(target new). ]
////                            ! [ stack push:(target new:arg). ].                                        
////                    ]
////                    ! [ stack push:arg. ].  
////                ]
////                "," ? [ ]
////                ! [
////                    counter += 1.
////                    
////                    stack push:(Symbol new &literal:(token literal)).
////                ].
////        ].
////        
////        ^ stack pop.
////    ]
////}
////
////#symbol configTest = 
////[
////    #var g := "mytest'C(mytest'A(),mytest'B())" loadConfiguration.
////    
////    g a.
////    g b.
////    g c.
////].

program =
[
//    SubjectSerializerTest eval.
//    
//    ClassSerializerTest eval.    

    InterpreterTest eval
            | if(:e)
                [
                    console writeLine:(e message); readChar.
                    
                    AbortException new; raise.
                ].

//    ClassTest eval.
////    ReflectionTester eval.
].