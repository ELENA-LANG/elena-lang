#define system.
#define system'routines.
#define system'io.
#define system'text.

// --- ReaderEnumerator ---

#class ReaderEnumerator :: Enumerator
{
    #field(type:textreader) theReader.
    #field theLine.

    #constructor new
    [
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
    
    #method reset 
    [
        theReader := StreamReader new &stream:(theReader stream clone) &encoder:(theReader encoder).
    ]
    
    #method next
    [
        (theReader available)
            ? [
                theLine := theReader read &literal.
                
                (nil equal:theLine)
                    ? [  theReader close. ^ false. ].
                    
                ^ true.            
            ].
        
        ^ false.
    ]
    
    #method get = theLine.
}
