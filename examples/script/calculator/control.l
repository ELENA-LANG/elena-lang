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
// --- testCase ---

#static factory
    = scriptEngine 
        load &path:"~\scripts\assembly.es" 
        load &path:"~\scripts\eson.es" 
        eval &path:"calc.es".

#symbol dynamicRules = 
    factory eval:(Library new) first eval.
    
#symbol staticRules =
    calc'rules'States.

#class(extension)testOp
{
    #method testEvaluation
    [
        #var result := self evaluated &with:dynamicRules.
        
        console writeLine:self:" evaluated as ":result.
    ]
    
    #method testStaticEvaluation
    [
        #var result := self evaluated &with:staticRules.
        
        console writeLine:self:" evaluated as ":result.
    ]
}

#symbol testCase =
[    
//    console writeLine:"Press any key to start" readChar.
    "22" testEvaluation.
    "22" testStaticEvaluation.

    //"2+3-1" testEvaluation.
    "2+3-1" testStaticEvaluation.
            
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
].

#symbol testProgram =
[
    #var machine := StateMachine new:dynamicRules seek &state0.
    #var machine2 := StateMachine new:staticRules seek &state0.
    
    machine digit:"1".
    machine2 digit:"1".
    
    #var retVal := machine value.
    #var retVal2 := machine2 value.
].

#symbol program =
[
].