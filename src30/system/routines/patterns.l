#define system.

// === patterns ===

// --- BasePattern ---

#class BasePattern : BaseFunction1
{
    #field theResult.
    
    #method run &enumerator:anEnumerator
    [
        $super run &enumerator:anEnumerator.
        
        ^ theResult.
    ]
}

// --- Summing ---

#class Summing : BasePattern
{
    #constructor new : aVariable
    [
        theResult := aVariable.
    ]

    #method eval : aParam [ theResult append:aParam. ]
}

//// --- DownCounter ---
//
//#class DownCounter
//{
//    #field theFunction.
//    
//    #constructor new : aFunction
//    [
//        theFunction := aFunction.
//    ]
//    
//    #method eval : aCounter
//    [
//        theFunction eval:aCounter.
//    ]
//    
//    #method eval &int:aCounter
//    [
//        theFunction eval:aCounter.
//    ]
//    
//    #method for : aCounter
//    [
//        $self~functionOp rewind &range &int::aCounter &int:1 &int:1.
//    ]
//}
//
