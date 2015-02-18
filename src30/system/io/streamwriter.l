#define system.
#define system'text.

// --- BinaryWriter ---

#class(limited) BinaryWriter
{
    #method stream [ #throw NotSupportedException new. ]

    #method close [ ]
    
    #method write &bytearray:anArray &length:aLength
    [
    ]

    #method write &byte:anInt
    [
        #var(type:bytearray,size:4)aBuffer.
        
        aBuffer write &index:0 &byte:anInt.
                
        $self write &bytearray:aBuffer &length:1.
    ]

    #method write &short:anInt
    [
        #var(type:bytearray,size:4)aBuffer.
        
        aBuffer write &index:0 &short:anInt.
                
        $self write &bytearray:aBuffer &length:2.
    ]

    #method write &char:anInt
    [
        #var(type:bytearray,size:4)aBuffer.
        
        aBuffer write &index:0 &char:anInt.
                
        $self write &bytearray:aBuffer &length:2.
    ]

    #method write &int:anInt
    [
        #var(type:bytearray,size:4)aBuffer.
        
        aBuffer write &index:0 &int:anInt.
                
        $self write &bytearray:aBuffer &length:4.
    ]

    #method write &long:aLong
    [
        #var(type:bytearray,size:8)aBuffer.
        
        aBuffer write &index:0 &long:aLong.
                
        $self write &bytearray:aBuffer &length:8.
    ]

    #method write &real:aLong
    [
        #var(type:bytearray,size:8)aBuffer.
        
        aBuffer write &index:0 &real:aLong.
                
        $self write &bytearray:aBuffer &length:8.
    ]
}

// --- TextWriter ---

#class(limited) TextWriter
{
    #field(type:literal)theNewLineConstant.
    
    #method stream [ #throw NotSupportedException new. ]

    #method encoder [ #throw NotSupportedException new. ]

    #method close [ ]    
    
    #method write &chararray:anArray &length:aLength
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
