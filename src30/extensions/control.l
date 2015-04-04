#define system.
#define system'routines.
#define system'threading.

#subject(class:system'BaseFunction1)forEach.
#subject(class:system'BaseFunction1)forEachWord.
#subject(class:system'BaseFunction1)forEachLine.
#subject(class:system'BaseFunction1)seeking.
#subject(class:system'IndexFunction1)forEvery.

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

    run:anEnumerable &forEach:aFunction
        = aFunction run &enumerator:(anEnumerable enumerator).

    run:anEnumerable &seeking:aFunction
        = aFunction retrieve &enumerator:(anEnumerable enumerator).

    find:aValue &in:aList
        = control run:aList &seeking: x [ aValue safeEqual:x ].

    ifExist:aValue &in:aList
        = nil != (control run:aList &seeking: x [ aValue equal:x ]).

    run &int:aStart &int:anEnd &forEvery:aFunction
        = (aStart < anEnd)
            ? [ aFunction run &range &int:aStart &int:anEnd &int:1 ]
            ! [ aFunction rewind &range &int:aStart &int:anEnd &int:1 ].
    
    run &int:aStart &int:anEnd &int:aStep &forEvery:aFunction
        = (aStart < anEnd)
            ? [ aFunction run &range &int:aStart &int:anEnd &int:aStep ]
            ! [ aFunction rewind &range &int:aStart &int:anEnd &int:aStep ].

    seek &int:aStart &int:anEnd &every:aFunction
        = aFunction retrieve &range &int:aStart &int:anEnd &int:1.

    seek &int:aStart &int:anEnd &int:aStep &every:aFunction
        = aFunction retrieve &range &int:aStart &int:anEnd &int:aStep.
        
    run:aText &forEachWord:anAction
        = anAction run &enumerator:(extensions'text'WordEnumerator new &literal:(aText literal)).
        
    run &path:aPath &forEachLine:anAction
        = anAction run &enumerator:(extensions'text'ReaderEnumerator new &path:aPath).

    sort : anIndexable
        = anIndexable~indexable sort:ifOrdered.

    getAt :anIndex &args:Params
    [
        #var(type:int)aLength := Params length.

        ^ Params@(aLength - anIndex int).
    ]
}.

// --- literalControl ---

#symbol literalControl =
{
    toArray : aLiteral
        = aLiteral ~indexable array.

    toCharArray : aLiteral
    [
        #var anArray := CharArray new &length:(aLiteral length).
        
        anArray write &index:0 &length:(aLiteral length) &literal:(aLiteral literal).
        
        ^ anArray.
    ]

    starting : anS &with:aSubS
    [
        #var(type:literal)s := anS literal.
        #var(type:literal)ss := aSubS literal.
        
        ^ 0 == s indexOf &index:0 &literal:ss.
    ]

    ending : anS &with:aSubS
    [
        #var(type:literal)s1 := anS literal.
        #var(type:literal)s2 := aSubS literal.
        
        #var(type:int)l1 := s1 length.
        #var(type:int)l2 := s2 length.
        
        #var(type:vint)n.
        s1 indexOf &index:0 &literal:s2 &vint:n.        
        
        ^ (l1 - l2) == n.
    ]
                
    contain : aSubS &in:anS
    [
        #var(type:literal)s := anS literal.
        #var(type:literal)ss := aSubS literal.
        
        ^ -1 != s indexOf &index:0 &literal:ss.
    ]
}.
/*
// --- threadControl ---

#symbol threadControl =
{
    start &func:anAction
    [
        #var aThread := Thread new &func:anAction.
        
        aThread start.
        
        ^ aThread.
    ]
}.
*/