// ==== ver 3.1.2 ===

#define system.
#define system'routines.

#class(extension:int)intLoopOp
{
    #method to &int:anEnd &doEvery:aFunction
    [
       #var(type:int)aStart := self int.

       ^ (aStart < anEnd)
            ? [ ^ aFunction run &int:aStart &int:anEnd &int:1. ]
            ! [ ^ aFunction rewind &int:aStart &int:anEnd &int:1. ].
    ]
}

#class(extension)forLoopOp
{
    #method to:aMaxValue &doEach:aFunction
        = aFunction run &enumerator:(RangeEnumerator new &from:self &to:aMaxValue).
        
    #method to:aMaxValue &seekEach:aFunction
        = aFunction retrieve &enumerator:(RangeEnumerator new &from:self &to:aMaxValue).
        
    #method to:aMaxValue &by:aStep &doEach:aFunction
        = aFunction run &enumerator:(RangeEnumerator new &from:self &to:aMaxValue &by:aStep).
        
    #method to:aMaxValue &by:aStep &seekEach:aFunction
        = aFunction retrieve &enumerator:(RangeEnumerator new &from:self &to:aMaxValue &by:aStep).
}

#class(extension)loopOp
{
    #method doWhile:anAction
    [
        #loop (self bool)?
        [
            anAction eval.
        ].
    ]
    
    #method doWhile
    [
        #loop (self bool)?
        [
        ].
    ]
}

// --- control ---

#symbol control =
{
    try : anAction
    [
        anAction eval | if: e [].
    ]    
    
    do:anAction = anAction eval.
    
    do:anAction &until:anExpr
        = anAction run &until:anExpr.

    while:anExpr
        = $self while:anExpr &do:[].

    while:anExpr &do:anAction
        = anAction run &if:anExpr.

    run &int:aStart &int:anEnd &every:aFunction
        = (aStart < anEnd)
            ? [ aFunction run &int:aStart &int:anEnd &int:1 ]
            ! [ aFunction rewind &int:aStart &int:anEnd &int:1 ].
    
    run &int:aStart &int:anEnd &int:aStep &every:aFunction
        = (aStart < anEnd)
            ? [ aFunction run &int:aStart &int:anEnd &int:aStep ]
            ! [ aFunction rewind &int:aStart &int:anEnd &int:aStep ].

    getAt :anIndex &args:Params
    [
        #var(type:int)aLength := Params length.

        ^ Params@(aLength - anIndex int).
    ]
}.
