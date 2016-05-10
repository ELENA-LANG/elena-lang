#import system.
#import system'routines.

//#class(extension:int)intLoopOp
//{
//    #method to &int:anEnd &doEvery:aFunction
//        = self::intLoopOp to &int:anEnd &nfunc:(aFunction nfunc).
//    
//    #method(stacksafe) to &int:anEnd &nfunc:aFunction
//    [
//       #var(type:int)aStart := self int.
//
//       ^ (aStart < anEnd)
//            ? [ ^ aFunction run &int:aStart &int:anEnd &int:1. ]
//            ! [ ^ aFunction rewind &int:aStart &int:anEnd &int:1. ].
//    ]
//}

#class(extension)forLoopOp
{
    #method to:aMaxValue &doEach:aFunction
        = aFunction run &enumerator:(RangeEnumerator new &from:self &to:aMaxValue).
        
    #method to:aMaxValue &by:aStep &doEach:aFunction
        = aFunction run &enumerator:(RangeEnumerator new &from:self &to:aMaxValue &by:aStep).
        
    #method till:aMaxValue &doEach:aFunction
        = aFunction run &enumerator:(IntervalEnumerator new &from:self &till:aMaxValue).
        
    #method till:aMaxValue &by:aStep &doEach:aFunction
        = aFunction run &enumerator:(IntervalEnumerator new &from:self &till:aMaxValue &by:aStep).
        
    #method to:aMaxValue &seekEach:aFunction
        = aFunction retrieve &enumerator:(RangeEnumerator new &from:self &to:aMaxValue).
        
    #method to:aMaxValue &by:aStep &seekEach:aFunction
        = aFunction retrieve &enumerator:(RangeEnumerator new &from:self &to:aMaxValue &by:aStep).
}

//#class(extension)loopOp
//{
//    #method doWhile:anAction
//    [
//        #loop (self bool)?
//        [
//            anAction eval.
//        ].
//    ]
//    
//    #method doWhile
//    [
//        #loop (self bool)?
//        [
//        ].
//    ]
//}
//
//// --- testOp ---
//
//#class(extension)testOp
//{
//    #method assert &ifTrue
//    [
//        self
//            ! [ console writeLine:"Assertion fails!" ].
//    ]
//    
//    #method assert &ifFalse
//    [
//        self
//            ? [ console writeLine:"Assertion fails!" ].
//    ]
//}

// --- controlOp ---

#class(extension)controlOp
{
    #method save &to:target
    [
        target write:self.
        
        ^ self.
    ]
}

// --- control ---

#symbol control =
{
    try : anAction &catching:aHandler
    [
        ^ anAction eval | if:aHandler.
    ]    
    
    do:anAction = anAction eval.
    
    do:anAction &until:anExpr
        = anAction run &until:anExpr.

    while:anExpr
        = $self while:anExpr &do:[].

    while:anExpr &do:anAction
        = anAction run &if:anExpr.

//    run &int:aStart &int:anEnd &every:aFunction
//        = (aStart < anEnd)
//            ? [ aFunction run &int:aStart &int:anEnd &int:1 ]
//            ! [ aFunction rewind &int:aStart &int:anEnd &int:1 ].
//    
//    run &int:aStart &int:anEnd &int:aStep &every:aFunction
//        = (aStart < anEnd)
//            ? [ aFunction run &int:aStart &int:anEnd &int:aStep ]
//            ! [ aFunction rewind &int:aStart &int:anEnd &int:aStep ].
}.
