#define system.

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
