#define system.

// --- BinaryWriter ---

#class(limited) BinaryWriter
{
    #method source = nil.

    #method close [ ]
    
    #method write &bytearray:anArray &length:aLength
    [
    ]

    #method write &int:anInt
    [
        #var(type:bytearray,size:4)aBuffer.
        
        aBuffer write &index:0 &int:anInt.
                
        $self write &bytearray:aBuffer &length:4.
    ]

    #method write &long:aLong
    [
        #var(type:bytearray,size:4)aBuffer.
        
        aBuffer write &index:0 &long:aLong.
                
        $self write &bytearray:aBuffer &length:8.
    ]
}

// --- TextWriter ---

#class(limited) TextWriter
{
    #field(type:literal)theNewLineConstant.
    
    #method source = nil.

    #method close [ ]
    
    
    #method write &shortarray:anArray &length:aLength
    [
    ]
    
    #method write &literal:anArray &length:aLength
    [
    ]
    
    #method writeLine : aLine
    [
        $self write &literal:(aLine literal) write &literal:theNewLineConstant.
    ]
    
    #method write &literal:aLine
    [
        #var(type:int)aLength := aLine length.
        
        $self write &literal:aLine &length:aLength.
    ]
}
