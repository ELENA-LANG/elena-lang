#define system.
#define extensions.

// --- controller ---

#symbol control =
{
    greeting = "Enter the expression to evaluate (e.g. 2+2) or -q to exit".
    
    prompt = ">".
    
    proceed : aLine
    [
        ("-q" == aLine)
            ? [ 'program stop. ^emptyLiteralValue. ]
            ! [
                ^ aLine evaluated.
            ].
    ]    
}.
//'program=extensions'programLoop
// --- testCase ---

#symbol testCase =
[    
//    console writeLine:"Press any key to start" readChar.
    
    #var aCase1 := "-2+3".
    
    #var aCaseResult1 := aCase1 evaluated.    
    console writeLine:aCase1:" evaluated as ":aCaseResult1.
    
    #var aCase2 := "2+3*4".
    
    #var aCaseResult2 := aCase2 evaluated.
    console writeLine:aCase2:" evaluated as ":aCaseResult2.
    
    #var aCase3 := "(2+3)*4".
    
    #var aCaseResult3 := aCase3 evaluated.
    console writeLine:aCase3:" evaluated as ":aCaseResult3.
].