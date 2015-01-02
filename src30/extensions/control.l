#define system.
//#define system'routines.
//#define system'dynamic.

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
        = aStart < anEnd
            ? [ aFunction run &range &int:aStart &int:anEnd &int:1 ]
            ! [ aFunction rewind &range &int:aStart &int:anEnd &int:1 ].
    
    run &int:aStart &int:anEnd &int:aStep &forEvery:aFunction
        = aStart < anEnd
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

    getAt :anIndex &args:Params
    [
        #var(type:int)aLength := Params length.

        ^ Params@(aLength - anIndex int).
    ]
}.
