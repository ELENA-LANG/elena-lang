import system'io.
import system'text.
import extensions'text.

// --- ReaderEnumerator ---

class ReaderEnumerator :: Enumerator
{
    textreader theReader.
    object     theLine.

    constructor new
    [ 
    ]

    multi constructor new : source
        <= new;
    [
        theReader := source textreader.
    ]

    constructor new stream:aStream encoder:anEncoder
        <= new;
    [
        theReader := StreamReader new stream:aStream encoder:anEncoder.
    ]        
                        
    constructor new literal:aPath
        <= new;
    [
        theReader := StreamReader new:aPath.
    ]
    
    constructor new literal:aPath encoder:anEncoder
        <= new;
    [
        theReader := StreamReader new literal:aPath encoder:anEncoder
    ]
    
    reset 
    [
        NotSupportedException new; raise.
    ]
    
    bool next
    [
        if(theReader available)
            [
                theLine := theReader readLiteral.
                
                ^ true
            ].
        
        theReader close.
        
        ^ false
    ]
    
    get = theLine.
}

// --- fileExOp ---

file_info extension fileExOp
{
    forEachLine:anAction
        = anAction run enumerator(ReaderEnumerator new:self).
        
//    run for:anEncoder eachLine:anAction
//        = anAction run enumerator:(ReaderEnumerator new path:self encoder:anEncoder).
}

// --- stringExOp ---

extension stringExOp
{
    forEachWord:aFunction
        = aFunction run enumerator(TokenEnumerator new literal:self).
        
//    run token_dfa:aStatemachine each:aFunction
//        = aFunction run enumerator:(TokenEnumerator new literal:self token_dfa:aStatemachine).
}

// --- printingLn ---

symbol printingLn = (:aLine)[ 'program'output writeLine:aLine ].

// --- func1Op ---

func1 extension func1Op
{
    asComparator
        = { bool equal : object = $self eval:object. }.
}