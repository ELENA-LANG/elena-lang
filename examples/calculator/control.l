#define system.
#define extensions.

//// --- controller ---
//
//#symbol control =
//{
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