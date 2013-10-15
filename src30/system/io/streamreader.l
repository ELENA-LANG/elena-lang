#define system.

// --- StreamReader ---

#class StreamReader
{
    #field theStream.
    
    #constructor new : aStream
    [
        theStream := aStream.
    ]
    
    #method read : anObject
    [
        $self read::anObject.
    ]
    
    #method read &type'widestr &type'length:aLength
    [
        #var aLiteral := LiteralValue new &type'length:aLength.
        
        theStream read &type'bytearray:aLiteral &type'length:(aLength * 2).
        
        system'external'CoreAPI refresh &type'widestr:aLiteral.
        
        ^ aLiteral.
    ]
}

// --- EStreamReader ---

#class EStreamReader
{    
    #method read &type'widestr &type'length:aLength
    [
        #var aLiteral := LiteralValue new &type'length:aLength.
        
        self read &type'bytearray:aLiteral &type'length:(aLength * 2).
        
        system'external'CoreAPI refresh &type'widestr:aLiteral.
        
        ^ aLiteral.
    ]

}