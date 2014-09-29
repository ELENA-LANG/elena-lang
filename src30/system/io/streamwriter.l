#define system.

// --- BinaryWriter ---

#class BinaryWriter
{
    #field theStream.
    
    #constructor new &stream:aStream
    [
        theStream := aStream.
    ]

    #method write &bytearray:anArray &length:aLength
    [
        theStream write &bytearray:anArray &length:aLength.
    ]

    #method write &int:anInt
    [
        #var(type:bytearray,size:4)aBuffer.
        
        aBuffer write &index:0 &int:anInt.
                
        theStream write &bytearray:aBuffer &length:4.
    ]

    #method write &long:aLong
    [
        #var(type:bytearray,size:4)aBuffer.
        
        aBuffer write &index:0 &long:aLong.
                
        theStream write &bytearray:aBuffer &length:8.
    ]
            
    #method close
    [
        theStream close.
    ]
}

// --- TextWriter ---

#class TextWriter
{
    #field(type:literal)theNewLineConstant.
    
    #method writeLine : aLine
    [
        $self write &literal::aLine write &literal:theNewLineConstant.
    ]
    
    #method write &literal:aLine
    [
        #var(type:int)aLength.
        aLine read &out'length:aLength.
        
        $self write &literal:aLine &length:aLength.
    ]
}

//
//// --- StreamWriter ---
//
//#class StreamWriter
//{
//    #field theStream.
//    
//    #constructor new : aStream
//    [
//        theStream := aStream.
//    ]
//
//    #method write : anObject
//    [
//        $self write::anObject.
//    ]
//
//    #method write &type'widestr:aLiteral
//    [
//        theStream write &type'bytearray:aLiteral &type'length:(aLiteral type'length * 2).
//    ]    
//}
