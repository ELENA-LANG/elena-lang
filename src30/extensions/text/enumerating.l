#define system.
#define system'routines.
#define system'io.

#type(class:system'BaseFunction1)forEachWord.
#type(class:system'BaseFunction1)forEachLine.

// --- ReaderEnumerator ---

#class ReaderEnumerator :: Enumerator
{
    #field(type:textreader) theReader.
    #field theLine.

    #constructor new
    [
    ]

    #constructor new &stream:aStream
        <= (%new)
    [
        theReader := StreamReader new &stream:aStream.
    ]        
                        
    #constructor new &path:aPath
        <= (%new)
    [
        theReader := StreamReader new &path:aPath.
    ]
    
    #method reset 
    [
        theReader := StreamReader new &stream:(theReader source clone).
    ]
    
    #method next
    [
        theLine := theReader readLine.
        
        ^ nil != theLine.
    ]
    
    #method get = theLine.
}
