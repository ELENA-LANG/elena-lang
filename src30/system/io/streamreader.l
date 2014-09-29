#define system.
#define system'routines.

// --- BinaryReader ---

#class BinaryReader
{
    #field theStream.
    
    #constructor new &stream:aStream
    [
        theStream := aStream.
    ]

    #method read &bytearray:anArray &length:aLength
    [
        #var(type:int)aRead := aLength.
        
        theStream read &bytearray:anArray &out'length:aRead.
        
        (aLength > aRead)
            ? [ #throw IOException new. ]
    ]

    #method read &out'int:anInt
    [
        #var(type:bytearray,size:4)aBuffer.
        #var(type:int)aRead := 4.
        
        theStream read &bytearray:aBuffer &out'length:aRead.
        
        (aRead < 4)
            ? [ #throw IOException new. ].
            
        aBuffer read &index:0 &out'int:anInt.
    ]

    #method read &out'long:aLong
    [
        #var(type:bytearray,size:8)aBuffer.
        #var(type:int)aRead := 8.
        
        theStream read &bytearray:aBuffer &out'length:aRead.
        
        (aRead < 8)
            ? [ #throw IOException new. ].
            
        aBuffer read &index:0 &out'long:aLong.
    ]
                                    
    #method close
    [
        theStream close.
    ]
}

// --- TextReader ---

#class TextReader
{
    #field theOutput.
    #field theOpen.

    #method available = theOpen.

    #method readLine
    [
        #var aLine := emptyLiteralValue.
        
        #var(type:int)aLength := 0.
        #loop
        [
            theOutput indexOf &index:0 &short:10 &out'int:aLength.
            
            (aLength == -1)
                ? [
                    #var(type:shortarray,size:64)aBuffer.
                    #var(type:int)aBufferLength := 64.
                    
                    $self read &shortarray:aBuffer &out'length:aBufferLength.
                    (aBufferLength == 0)
                        ? [ theOpen := false. ]
                        ! [ theOutput write &shortarray:aBuffer &length:aBufferLength. ].
                ].
                
            theOpen and:(aLength == -1)
        ].
        
        (aLength == -1)
            ? [
                aLine := theOutput literal.                
                theOutput clear.
                
                theOpen := aLine~literalOp isNotEmpty.
            ]
            ! [ 
                aLine := theOutput Substring &index:0 &length:(aLength - 1).
                theOutput delete &index:0 &length:(1 + aLength).
            ].            

        ^ aLine.
    ]
}

