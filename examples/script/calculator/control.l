#import system'dynamic.
#import extensions.
#import extensions'scripting.

////'program=extensions'programLoop
// --- testCase ---

static factory
    = scriptEngine 
        load &path:"~\scripts\assembly.es" 
        load &path:"~\scripts\eson.es" 
        eval &path:"calc.es".

symbol dynamicRules = 
    factory eval:(Library new) first eval.
    
symbol staticRules =
    calc'rules'States.

extension testOp
{
    testEvaluation
    [
        var result := self evaluated &with:dynamicRules.
        
        console writeLine:self:" evaluated as ":result.
    ]
    
    testStaticEvaluation
    [
        var result := self evaluated &with:staticRules.
        
        console writeLine:self:" evaluated as ":result.
    ]
}

testCases =
[    
    "22.4" testEvaluation.

    "2+3-1" testEvaluation.
            
    "-2+3" testEvaluation.

    "2+3*4" testEvaluation.

    "(2+3)*4" testEvaluation.
    
    "sqrt(2*2)" testEvaluation.
    
    "min(2,3)" testEvaluation.
].

staticTestCases =
[    
    "22.4" testStaticEvaluation.

    "2+3-1" testStaticEvaluation.
            
    "-2+3" testEvaluation.

    "2+3*4" testStaticEvaluation.

    "(2+3)*4" testStaticEvaluation.
    
    "sqrt(2*2)" testEvaluation.    
].

// --- controller ---

class control =
{
    greeting = "Enter the expression to evaluate (e.g. 2+2) or -q to exit".
    
    prompt = ">".
    
    proceed : aLine
    [
        ("-q" == aLine)
            ? [ 'program stop. ^emptyLiteralValue. ]
            ! [
                ^ aLine evaluated &with:'rules.
            ].
    ]    
}.