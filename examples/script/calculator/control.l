import system'dynamic.
import extensions.
import extensions'scripting.
import extensions'dynamic.

////'program=extensions'programLoop
// --- testCase ---

public static dynamicRules = 
    jscript evalPath:"calc.es".
    
public symbol staticRules =
    calc'rules'States.

extension testOp
{
    testEvaluation
    [
        var result := self evaluatedWith:dynamicRules.
        
        console printLine(self," evaluated as ",result).
    ]    
}

extension staticTestOp
{
    testStaticEvaluation
    [
        var result := self evaluatedWith:staticRules.
        
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

public class control =
{
    greeting = "Enter the expression to evaluate (e.g. 2+2) or -q to exit".
    
    prompt = ">".
    
    proceed : aLine
    [
        if ("-q" == aLine)
            [ forwards'program stop. ^emptyLiteral ];
            [
                ^ aLine evaluatedWith:forwards'rules
            ].
    ]    
}.