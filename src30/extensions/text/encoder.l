//#define system.
//#define system'text.
//
//// --- encoderOp ---
//
//#class(extension:encoder)encoderOp
//{
//    #method convert &bytearray:aBuffer &length:aLength &textbuffer:anOutput
//    [
//        #var(type:encoder)anEncoder := self encoder.
//                
//        #var(type:int)aCharLen := 255.
//        #var(type:chararray,size:255)aCharBuffer.
//        
//        #var(type:int)anIndex := 0.
//        #var(type:int)aPage := 255.
//        
//        #loop (anIndex < aLength)?
//        [
//            (aPage > (aLength - anIndex))
//                ? [ aPage := aLength - anIndex. ].
//            
//            anEncoder convert &index:anIndex &vlength:aPage &bytearray:aBuffer &chararray:aCharBuffer &vlength:aCharLen.
//            anIndex := anIndex + aPage.
//            
//            anOutput write &chararray:aCharBuffer &length:aCharLen.
//        ].
//    ]
//    
//    #method toLiteral &bytearray:aBuffer &length:aLength
//    [
//        #var(type:textbuffer)anOutput := TextBuffer new.
//        self::encoderOp convert &bytearray:aBuffer &length:aLength &textbuffer:anOutput.
//        
//        ^ anOutput literal.
//    ]
//    
//    #method toWide &bytearray:aBuffer &length:aLength
//    [
//        #var(type:textbuffer)anOutput := TextBuffer new.
//        self::encoderOp convert &bytearray:aBuffer &length:aLength &textbuffer:anOutput.
//        
//        ^ anOutput wide.
//    ]
//}