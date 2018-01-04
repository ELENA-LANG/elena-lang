import system'dynamic.
import extensions.
import extensions'scripting.
import extensions'dynamic.

////'program=extensions'programLoop
// --- testCase ---

static assembly_tape
    = scriptEngine 
        load path:"~\scripts\assembly.es";
        load path:"~\scripts\jscript.es";
        eval path:"calc.es".

symbol dynamicRules = 
    TapeAssembly new(assembly_tape); eval.
    
symbol staticRules =
    calc'rules'States.

extension testOp
{
    testEvaluation
    [
        var result := self evaluated with:dynamicRules.
        
        console printLine(self," evaluated as ",result).
    ]    
}

extension staticTestOp
{
    testStaticEvaluation
    [
        var result := self evaluated with:staticRules.
        
        console printLine(self," evaluated as ",result)
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
        if ("-q" == aLine)
            [ 'program stop. ^emptyLiteralValue ];
            [
                ^ aLine evaluated with:'rules
            ].
    ]    
}.