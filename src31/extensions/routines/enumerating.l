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

    constructor new : source
        <= new;
    [
        theReader := source textreader.
    ]

    constructor new stream:aStream encoder:anEncoder
        <= new;
    [
        theReader := StreamReader new stream:aStream encoder:anEncoder.
    ]        
                        
    constructor new path:aPath
        <= new;
    [
        theReader := StreamReader new path:aPath.
    ]
    
    constructor new path:aPath encoder:anEncoder
        <= new;
    [
        theReader := StreamReader new path:aPath encoder:anEncoder
    ]
    
    reset 
    [
        NotSupportedException new; raise.
    ]
    
    next
    [
        if(theReader available)
            [
                theLine := theReader read literal.
                
                ^ true
            ].
        
        theReader close.
        
        ^ false
    ]
    
    get = theLine.
}

// --- fileExOp ---

file_path extension fileExOp
{
    run eachLine:anAction
        = anAction run enumerator:(ReaderEnumerator new path:self).
        
    run for:anEncoder eachLine:anAction
        = anAction run enumerator:(ReaderEnumerator new path:self encoder:anEncoder).
}

// --- stringExOp ---

extension stringExOp
{
    run eachWord:aFunction
        = aFunction run enumerator:(TokenEnumerator new literal:self).
        
    run token_dfa:aStatemachine each:aFunction
        = aFunction run enumerator:(TokenEnumerator new literal:self token_dfa:aStatemachine).
}

// --- printingLn ---

symbol printingLn = (:aLine)[ 'program'output writeLine:aLine ].

// --- func1Op ---

func1 extension func1Op
{
    asComparer
        = { equal : object = $owner eval:object. }.
}