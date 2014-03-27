#define system.

// --- StreamWriter ---

#class StreamWriter
{
    #field theStream.
    
    #constructor new : aStream
    [
        theStream := aStream.
    ]

    #method write : anObject
    [
        $self write::anObject.
    ]

    #method write &type'widestr:aLiteral
    [
        theStream write &type'bytearray:aLiteral &type'length:(aLiteral type'length * 2).
    ]    
}
