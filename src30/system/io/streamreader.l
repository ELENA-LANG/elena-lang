//#define system.
//
//// --- StreamReader ---
//
//#class StreamReader
//{
//    #field theStream.
//    
//    #constructor new : aStream
//    [
//        theStream := aStream.
//    ]
//    
//    #method read : anObject
//    [
//        $self read::anObject.
//    ]
//    
//    #method read &type'widestr &type'length:aLength
//    [
//        #var aLiteral := LiteralValue new &type'length:aLength.
//        
//        theStream read &type'bytearray:aLiteral &type'length:(aLength * 2).
//        
//        strOp refresh &type'widestr:aLiteral.
//        
//        ^ aLiteral.
//    ]
//}
