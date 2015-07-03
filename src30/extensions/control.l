// ==== ver 3.1.2 ===

#define system.
#define system'routines.
//#define system'text.

#subject(class:system'IndexFunction1) doEvery.
#subject(class:system'BaseFunction1)  doEach.
#subject(class:system'BaseFunction1)  seekEach.

//#subject(class:system'BaseFunction1)seeking.
//#subject(class:system'BaseFunction2)forEachPair.

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

#class(extension)loopOp
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

//    run:anEnumerable &forEach:aFunction
//        = aFunction run &enumerator:(anEnumerable enumerator).
//
//    run:anEnumerable &with:aParam &forEachPair:aFunction
//        = aFunction run:aParam &enumerator:(anEnumerable enumerator).
//
//    run:anEnumerable &seeking:aFunction
//        = aFunction retrieve &enumerator:(anEnumerable enumerator).
//
//    find:aValue &in:aList
//        = control run:aList &seeking: x [ aValue safeEquale:x ].
//
//    ifExist:aValue &in:aList
//        = $nil != (control run:aList &seeking: x [ aValue equal:x ]).
//
//    seek &int:aStart &int:anEnd &every:aFunction
//        = aFunction retrieve &int:aStart &int:anEnd &int:1.
//
//    seek &int:aStart &int:anEnd &int:aStep &every:aFunction
//        = aFunction retrieve &int:aStart &int:anEnd &int:aStep.
//        
//    run:aText &forEachWord:anAction
//        = anAction run &enumerator:(extensions'text'WordEnumerator new &literal:(aText literal)).
//        
//    run &path:aPath &encoder:anEncoder &forEachLine:anAction
//        = anAction run &enumerator:(extensions'text'ReaderEnumerator new &path:aPath &encoder:anEncoder).

//    sort : anIndexable
//        = anIndexable~indexable sort:ifOrdered.

    getAt :anIndex &args:Params
    [
        #var(type:int)aLength := Params length.

        ^ Params@(aLength - anIndex int).
    ]
}.

//// --- literalControl ---
//
//#symbol literalControl =
//{
//    toArray : aLiteral
//        = aLiteral ~indexable array.
//
//    toCharArray : aLiteral
//    [
//        #var anArray := CharArray new &length:(aLiteral length).
//        
//        anArray write &index:0 &length:(aLiteral length) &literal:(aLiteral literal).
//        
//        ^ anArray.
//    ]
//
//    starting : anS &with:aSubS
//    [
//        #var(type:literal)s := anS literal.
//        #var(type:literal)ss := aSubS literal.
//        
//        ^ 0 == s indexOf &index:0 &literal:ss.
//    ]
//
//    ending : anS &with:aSubS
//    [
//        #var(type:literal)s1 := anS literal.
//        #var(type:literal)s2 := aSubS literal.
//        
//        #var(type:int)l1.
//        s1 readLength &vint:l1.
//        #var(type:int)l2.
//        s2 readLength &vint:l2.
//        
//        #var(type:int)n.
//        s1 indexOf &index:0 &literal:s2 &vint:n.        
//        
//        ^ (l1 - l2) == n.
//    ]
//                
//    contain : aSubS &in:anS
//    [
//        #var(type:literal)s := anS literal.
//        #var(type:literal)ss := aSubS literal.
//
//        #var(type:int)n.
//        s indexOf &index:0 &literal:ss &vint:n.
//                
//        ^ -1 != n.
//    ]
//}.
