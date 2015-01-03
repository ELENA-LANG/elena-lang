#define system.
#define system'routines.
#define system'io.

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
        theReader := StreamReader new &stream:(theReader source clone) &encoder:(theReader encoder).
    ]
    
    #method next
    [
        theLine := theReader readLine.
        
        ^ nil equal:theLine not.
    ]
    
    #method get = theLine.
}
