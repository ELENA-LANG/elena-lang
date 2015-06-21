#define system.
#define system'routines.
#define system'text.

// --- BinaryReader ---

#class(limited) BinaryReader
{
    #field(type:stream)theStream.
    #field(type:int)theLength.
    
    #constructor new &stream:aStream
    [
        theStream := aStream.
        
        theLength := aStream length.
    ]
    
    #method read &bytearray:anArray &vlength:anActualLength
    [
        theStream read &bytearray:anArray &vlength:anActualLength.
    ]

    #method stream = theStream.

    #method close => theStream.

    #method(type:bool) available
    [
        #var(type:int)aPosition.
        
        theStream readIndex &vint:aPosition.
        
        ^ aPosition < theLength.
    ]
            
    #method read &bytearray:anArray &length:aLength
    [
        #var(type:length)aRead := aLength.
        
        $self read &bytearray:anArray &vlength:aRead.
         
        (aLength > aRead)
            ? [ #throw IOException new. ].
    ]

    #method read &vbyte:anInt
    [
        #var(type:bytearray,size:4)aBuffer.
        #var(type:length)aRead := 1.
        
        $self read &bytearray:aBuffer &vlength:aRead.
        
        (aRead < 1)
            ? [ #throw IOException new. ].
            
        aBuffer read &index:0 &vbyte:anInt.
    ]

    #method read &vshort:anInt
    [
        #var(type:bytearray,size:4)aBuffer.
        #var(type:length)aRead := 2.
        
        $self read &bytearray:aBuffer &vlength:aRead.
        
        (aRead < 1)
            ? [ #throw IOException new. ].
            
        aBuffer read &index:0 &vshort:anInt.
    ]

    #method read &vchar:anInt
    [
        #var(type:bytearray,size:4)aBuffer.
        #var(type:length)aRead := 4.
        
        $self read &bytearray:aBuffer &vlength:aRead.
        
        (aRead < 1)
            ? [ #throw IOException new. ].
            
        aBuffer read &index:0 &vchar:anInt.
    ]

    #method read &vint:anInt
    [
        #var(type:bytearray,size:4)aBuffer.
        #var(type:length)aRead := 4.
        
        $self read &bytearray:aBuffer &length:aRead.
        
        (aRead < 4)
            ? [ #throw IOException new. ].
            
        aBuffer read &index:0 &vint:anInt.
    ]

    #method read &vlong:aLong
    [
        #var(type:bytearray,size:8)aBuffer.
        #var(type:length)aRead := 8.
        
        $self read &bytearray:aBuffer &vlength:aRead.
        
        (aRead < 8)
            ? [ #throw IOException new. ].
            
        aBuffer read &index:0 &vlong:aLong.
    ]

    #method read &vreal:aReal
    [
        #var(type:bytearray,size:8)aBuffer.
        #var(type:length)aRead := 8.
        
        $self read &bytearray:aBuffer &vlength:aRead.
        
        (aRead < 8)
            ? [ #throw IOException new. ].
            
        aBuffer read &index:0 &vreal:aReal.
    ]
}

// --- TextReader ---

#class(limited) TextReader
{
    #field(type:textbuffer) theOutput.
    #field(type:literal)    theNewLineConstant.
    #field(type:int)        theNewLineLength.

    #constructor new
    [
        theOutput := TextBuffer new.
        theNewLineConstant := 'newLine literal.
                
        theNewLineLength := theNewLineConstant length.
    ]

    #method read &chararray:anArray &vlength:anActualLength
    [
    ]

    #method stream [ #throw NotSupportedException new. ]

    #method encoder [ #throw NotSupportedException new. ]

    #method close [ ]
    
    #method(type:bool) available = false.
            
    #method(type:literal) read &literal
    [
        #var(type:literal) aLine := emptyLiteralValue.
        
        #var(type:int)aLength := -1.
        #loop (aLength == -1) ?
        [
            theOutput indexOf &index:0 &literal:theNewLineConstant &vint:aLength.
            
            (aLength == -1)
                ? [
                    #var(type:chararray,size:64)aBuffer.
                    #var(type:length)aBufferLength := 64.
                    
                    $self read &chararray:aBuffer &vlength:aBufferLength.
                    (aBufferLength == 0)
                        ? [ aLength := -2. ]  // !! to indicate the eof
                        ! [ theOutput write &chararray:aBuffer &length:aBufferLength. ].
                ].
        ].
        
        (aLength < 0) 
            ? [
                aLine := theOutput literal.                
                theOutput clear.
            ]
            ! [ 
                aLine := theOutput Substring &index:0 &length:aLength.
                theOutput delete &index:0 &length:(aLength + theNewLineLength).
            ].            

        ^ aLine.
    ]
    
    #method(type:wide) read &wide
    [
        #var(type:wide) aLine := emptyWideLiteralValue.
        
        #var(type:int)aLength := -1.
        #loop (aLength == -1) ?
        [
            theOutput indexOf &index:0 &char:10 &vint:aLength.
            
            (aLength == -1)
                ? [
                    #var(type:chararray,size:64)aBuffer.
                    #var(type:vlength)aBufferLength := 64.
                    
                    $self read &chararray:aBuffer &vlength:aBufferLength.
                    (aBufferLength == 0)
                        ? [ aLength := -2. ]  // !! to indicate the eof
                        ! [ theOutput write &chararray:aBuffer &length:aBufferLength. ].
                ].
        ].
        
        (aLength <= 0) 
            ? [
                aLine := theOutput wide.                
                theOutput clear.
            ]
            ! [ 
                aLine := theOutput WideSubstring &index:0 &length:(aLength - 1).
                theOutput delete &index:0 &length:(1 + aLength).
            ].            

        ^ aLine.
    ]
}
