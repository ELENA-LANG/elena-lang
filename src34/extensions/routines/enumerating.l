import system'io.
import system'text.
import extensions.
import extensions'text.

// --- ReaderEnumerator ---

class ReaderEnumerator :: Enumerator
{
    T<TextReader> theReader.
    object        theLine.

    constructor new
    [ 
    ]

    constructor new : source
        <= new;
    [
        theReader := source textreader.
    ]

    constructor new(Stream aStream, Encoder anEncoder)
        <= new;
    [
        theReader := StreamReader new(aStream, anEncoder).
    ]        
                        
    constructor new(literal aPath)
        <= new;
    [
        theReader := StreamReader new:aPath.
    ]
    
    constructor new(literal aPath, Encoder anEncoder)
        <= new;
    [
        theReader := StreamReader new(aPath, anEncoder)
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

    enumerable = theReader.
            
    get = theLine.
}

// --- fileExOp ---

public extension<File> fileExOp
{
    forEachLine:anAction
        = anAction run(ReaderEnumerator new:self).
        
//    run for:anEncoder eachLine:anAction
//        = anAction run enumerator:(ReaderEnumerator new path:self encoder:anEncoder).
}

// --- stringExOp ---

public extension stringExOp
{
    forEachWord(Func1 aFunction)
        = aFunction run(T<Enumerator>(TokenEnumerator new(self))).
        
//    run token_dfa:aStatemachine each:aFunction
//        = aFunction run enumerator:(TokenEnumerator new literal:self token_dfa:aStatemachine).
}

// --- printingLn ---

public symbol printingLn = (:aLine)[ forwards'program'output printLine:aLine ].

// --- func1Op ---

public extension<Func1> func1Op
{
    asComparator
        = { bool equal : object = self(object). }.
}