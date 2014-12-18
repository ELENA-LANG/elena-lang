#define system.
#define system'routines.
#define system'text.

//// --- BinaryReader ---
//
//#class BinaryReader
//{
//    #method read &bytearray:anArray &length:aLength
//    [
//        #var(type:int)aRead := aLength.
//        
//        theStream read &bytearray:anArray &out'length:aRead.
//        
//        (aLength > aRead)
//            ? [ #throw IOException new. ]
//    ]
//
//    #method read &out'int:anInt
//    [
//        #var(type:bytearray,size:4)aBuffer.
//        #var(type:int)aRead := 4.
//        
//        theStream read &bytearray:aBuffer &out'length:aRead.
//        
//        (aRead < 4)
//            ? [ #throw IOException new. ].
//            
//        aBuffer read &index:0 &out'int:anInt.
//    ]
//
//    #method read &out'long:aLong
//    [
//        #var(type:bytearray,size:8)aBuffer.
//        #var(type:int)aRead := 8.
//        
//        theStream read &bytearray:aBuffer &out'length:aRead.
//        
//        (aRead < 8)
//            ? [ #throw IOException new. ].
//            
//        aBuffer read &index:0 &out'long:aLong.
//    ]
//                                    
//    #method close
//    [
//        theStream close.
//    ]
//}

// --- TextReader ---

#class(limited,type:textreader) TextReader
{
    #field(type:textbuffer) theOutput.

    #constructor new
    [
        theOutput := TextBuffer new.
    ]

    #method read &shortarray:anArray &length:aMaxLength &out'length:anActualLength
    [
    ]

    #method source = nil.

    #method close [ ]
    
    #method readLine
    [
        #var(type:literal) aLine := emptyLiteralValue.
        
        #var(type:int)aLength := -1.
        #loop (aLength == -1) ?
        [
            theOutput indexOf &index:0 &short:10 &out'int:aLength.
            
            (aLength == -1)
                ? [
                    #var(type:shortarray,size:64)aBuffer.
                    #var(type:int)aBufferLength := 64.
                    
                    $self read &shortarray:aBuffer &length:aBufferLength &out'length:aBufferLength.
                    (aBufferLength == 0)
                        ? [ aLength := -2. ]  // !! to indicate the eof
                        ! [ theOutput write &shortarray:aBuffer &length:aBufferLength. ].
                ].
        ].
        
        (aLength <= 0) 
            ? [
                aLine := theOutput literal.                
                theOutput clear.
                
                aLine isEmpty
                    ? [ ^ nil. ].
            ]
            ! [ 
                aLine := theOutput Substring &index:0 &length:(aLength - 1).
                theOutput delete &index:0 &length:(1 + aLength).
            ].            

        ^ aLine.
    ]
}
