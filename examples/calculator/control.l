#define system.

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
    
    #var aCase2 := "2+3".
    
    #var aCaseResult2 := aCase2 evaluated.
].