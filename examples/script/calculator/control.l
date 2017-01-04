#import system.

#import system'dynamic.
#import extensions.
#import extensions'scripting.

//// --- controller ---
//
//#symbol control =
//{
//    greeting = "Enter the expression to evaluate (e.g. 2+2) or -q to exit".
//    
//    prompt = ">".
//    
//    proceed : aLine
//    [
//        ("-q" == aLine)
//            ? [ 'program stop. ^emptyLiteralValue. ]
//            ! [
//                ^ aLine evaluated.
//            ].
//    ]    
//}.
////'program=extensions'programLoop
//// --- testCase ---
//
//#symbol testCase =
//[    
////    console writeLine:"Press any key to start" readChar.
//    
//    #var aCase1 := "-2+3".
//    
//    #var aCaseResult1 := aCase1 evaluated.    
//    console writeLine:aCase1:" evaluated as ":aCaseResult1.
//    
//    #var aCase2 := "2+3*4".
//    
//    #var aCaseResult2 := aCase2 evaluated.
//    console writeLine:aCase2:" evaluated as ":aCaseResult2.
//    
//    #var aCase3 := "(2+3)*4".
//    
//    #var aCaseResult3 := aCase3 evaluated.
//    console writeLine:aCase3:" evaluated as ":aCaseResult3.
//].

#symbol testProgram =
[
    #var state1 := Struct(%digit, 
        Tape(
            { order = 1. evalToken : content = content toReal. },
            1, %"tapeOp.var[]",
            %"newLeaf[1]",
            2, %"tapeOp.var[]",
            1, %"tapeOp.var[]",
            %"addContent[1]",
            1, %"tapeOp.var[]",
            %"seek&state0"
        )).
                
    #var states := Struct(%state0, state1).

    #var states_alt := scriptEngine
        eval &path:"test.es".

    #var machine := StateMachine new:states seek &state0.
    #var machine2 := StateMachine new:states_alt seek &state0.
    
    machine digit:"1".
    machine2 digit:"1".
    
    #var retVal := machine value.
    #var retVal2 := machine2 value.
].

#symbol program =
[
].