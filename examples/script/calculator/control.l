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

// --- testCase ---

#symbol testCase =
[
    #var aCase1 := "2".
    
    #var aCaseResult1 := aCase1 evaluated.    
    console writeLine:aCase1:" evaluated as ":aCaseResult1.
    
    #var aCase2 := "12+3".
    
    #var aCaseResult2 := aCase2 evaluated.
    console writeLine:aCase2:" evaluated as ":aCaseResult2.
].