// ==== ver 3.4.1 ===

#import system.
#import system'io.
//#define system'text.
#import extensions'text.

// --- ReaderEnumerator ---

#class ReaderEnumerator :: Enumerator
{
    #field(type:textreader) theReader.
    #field theLine.

    #constructor new
    [ 
    ]

    #constructor new : source
        <= (new)
    [
        theReader := source textreader.
    ]

    #constructor new &stream:aStream &encoder:anEncoder
        <= (%new)
    [
        theReader := StreamReader new &stream:aStream &encoder:anEncoder.
    ]        
                        
    #constructor new &path:aPath
        <= (%new)
    [
        theReader := StreamReader new &path:aPath.
    ]
    
    #constructor new &path:aPath &encoder:anEncoder
        <= (%new)
    [
        theReader := StreamReader new &path:aPath &encoder:anEncoder.
    ]
    
    #method reset 
    [
        #throw NotSupportedException new.
    ]
    
    #method next
    [
        (theReader available)
            ? [
                theLine := theReader read &literal.
                
                ^ true.            
            ].
        
        theReader close.
        
        ^ false.
    ]
    
    #method get = theLine.
}

// --- fileExOp ---

#class(extension:file_path)fileExOp
{
    #method run &eachLine:anAction
        = anAction run &enumerator:(ReaderEnumerator new &path:self).
}

// --- stringExOp ---

#class(extension)stringExOp
{
    #method run &eachWord:aFunction
        = aFunction run &enumerator:(TokenEnumerator new &literal:(self literal)).
        
    #method run &token_dfa:aStatemachine &each:aFunction
        = aFunction run &enumerator:(TokenEnumerator new &literal:(self literal) &token_dfa:aStatemachine).
}

// --- printingLn ---

#symbol printingLn = aLine [ 'program'output writeLine:aLine ].

// --- func1Op ---

#class(extension:func1)func1Op
{
    #method asComparer
        = { equal : object = self eval:object. }.
}