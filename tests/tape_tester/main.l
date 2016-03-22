#import system.
#import system'routines.
#import system'dynamic.
#import extensions'dynamic.

#symbol InterpreterTest =
[
    scriptEngine load &path:"~\scripts\eson.es".

    //#var s := scriptEngine eval:"{ x : 1 , y : 2 , writeLine : function(x){ system'console.writeLine(x) } }".
    #var s := scriptEngine eval:"{ x : 1 , y : 2 }".
    //#var s := scriptEngine eval:"{ x : 1 }".
    
    #var x := s x.
    #var y := s y.
    
//    ((x == 1)and:(y == 2))?
//    [
//        console writeLine:"Script: structure works".
//    ].
    
    s writeLine:"Script: function works".
].

#symbol ReflectionTester =
[
    #var s := 2 toTextSerialized. 
        
    #var anObject := String new:"abd".

    #var aPresentation := anObject toTextSerialized. 
].

#symbol ClassTest =
[
    #var n := Integer new.
  
    #var g := Struct(%getNext, Tape(1, 1, %"tapeOp.var[]", %"get&index", %"append[1]"), %index, n).        

    g eval &getNext.
].

#symbol SubjectSerializerTest =
[
    #var subj := %length.
    
    #var subjName := %length :: subjectROp name.
    
    #var newSubj := Signature new &literal:subjName.
    
    (subj equal &subject:newSubj)
        ? [ console writeLine:"Subject serializing works". ]
        ! [ console writeLine:"Subject serializing does not work". ].
].

#symbol ClassSerializerTest =
[
    #var aClass := Symbol new &literal:"system'Integer".
    #var anInstance := aClass new.
    
    #var className := anInstance className.
    
    (className == "system'Integer")
        ? [ console writeLine:"Class serializing works". ]
        ! [ console writeLine:"Class serializing does not work". ].    
].

#symbol Program =
[ 
    //SubjectSerializerTest eval.
    
    //ClassSerializerTest eval.    

    InterpreterTest eval
        | if &ScriptError: e [ console writeLine:(e message). ].

    //ClassTest eval.
    //ReflectionTester eval.
].