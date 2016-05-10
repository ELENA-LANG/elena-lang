#import system.
#import system'routines.
#import system'text.

// --- BinaryReader ---

#class(limited) BinaryReader
{
    #field(stream)theStream.
    #field(int)   theLength.
    
    #constructor new &stream:aStream
    [
        theStream := aStream.
        
        theLength := aStream length.
    ]
    
    #method(stacksafe) read &bytearray:anArray &vint:anActualLength
    [
        theStream read &bytearray:anArray &vint:anActualLength.
    ]

    #method stream = theStream.

    #method close => theStream.

    #method(bool) available
    [
        #var(int)aPosition := theStream index.
        
        ^ aPosition < theLength.
    ]
            
    #method(stacksafe) read &bytearray:anArray &int:aLength
    [
        #var(int)aRead := aLength.
        
        $self read &bytearray:anArray &vint:aRead.
         
        (aLength > aRead)
            ? [ #throw IOException new. ].
    ]

    #method read &vbyte:anInt
    [
        #var(byte:4)aBuffer.
        #var(int)aRead := 1.
        
        $self read &bytearray:aBuffer &vint:aRead.
        
        (aRead < 1)
            ? [ #throw IOException new. ].
     
        #var(byte)b := aBuffer@0.
                          
        anInt := b.
    ]

    #method read &vshort:anInt
    [
        #var(byte:4)aBuffer.
        #var(int)aRead := 2.
        
        $self read &bytearray:aBuffer &vint:aRead.
        
        (aRead < 1)
            ? [ #throw IOException new. ].
            
        byteArrayHelper read &bytearray:aBuffer &int:0 &vshort:anInt.
    ]

    #method read &vchar:anInt
    [
        #var(byte:4)aBuffer.
        #var(int)aRead := 4.
        
        $self read &bytearray:aBuffer &vint:aRead.
        
        (aRead < 1)
            ? [ #throw IOException new. ].
            
        byteArrayHelper read &bytearray:aBuffer &int:0 &vchar:anInt.
    ]

    #method read &vint:anInt
    [
        #var(byte:4)aBuffer.
        #var(int)aRead := 4.
        
        $self read &bytearray:aBuffer &int:aRead.
        
        (aRead < 4)
            ? [ #throw IOException new. ].
            
        byteArrayHelper read &bytearray:aBuffer &int:0 &vint:anInt.
    ]

    #method read &vlong:aLong
    [
        #var(byte:8)aBuffer.
        #var(int)aRead := 8.
        
        $self read &bytearray:aBuffer &vint:aRead.
        
        (aRead < 8)
            ? [ #throw IOException new. ].
            
        byteArrayHelper read &bytearray:aBuffer &int:0 &vlong:aLong.
    ]

    #method read &vreal:aReal
    [
        #var(byte:8)aBuffer.
        #var(int)aRead := 8.
        
        $self read &bytearray:aBuffer &vint:aRead.
        
        (aRead < 8)
            ? [ #throw IOException new. ].
            
        byteArrayHelper read &bytearray:aBuffer &int:0 &vreal:aReal.
    ]
}

// --- TextReader ---

#class(limited) TextReader
{
    #field(textbuffer) theOutput.
    #field(literal)    theNewLineConstant.
    #field(int)        theNewLineLength.

    #constructor new
    [
        theOutput := TextBuilder new.
        theNewLineConstant := 'newLine literal.
                
        theNewLineLength := theNewLineConstant length.
    ]

    #method(stacksafe) read &chararray:anArray &vint:anActualLength
    [
    ]

    #method stream [ #throw NotSupportedException new. ]

    #method encoder [ #throw NotSupportedException new. ]

    #method close [ ]
    
    #method(bool) available = false.
            
    #method(literal) read &literal
    [
        #var(literal) aLine := emptyLiteralValue.
        
        #var(int)aLength := -1.
        #loop (aLength == -1) ?
        [
            theOutput indexOf &int:0 &literal:theNewLineConstant &vint:aLength.
            
            (aLength == -1)
                ? [
                    #var(char:64)aBuffer.
                    #var(int)aBufferLength := 64.
                    
                    $self read &chararray:aBuffer &vint:aBufferLength.
                    (aBufferLength == 0)
                        ? [ aLength := -2. ]  // !! to indicate the eof
                        ! [ theOutput write &chararray:aBuffer &int:aBufferLength. ].
                ].
        ].
        
        (aLength < 0) 
            ? [
                aLine := theOutput literal.                
                theOutput clear.
            ]
            ! [ 
                aLine := theOutput Substring &int:0 &int:aLength.
                theOutput delete &int:0 &int:(aLength + theNewLineLength).
            ].            

        ^ aLine.
    ]
    
    #method(wide) read &wide
    [
        #var(wide) aLine := emptyWideLiteralValue.
        #var(wide) nl := theNewLineConstant wide.
        #var(int) nl_len.
        nl readLength &vint:nl_len.
        
        #var(int)aLength := -1.
        #loop (aLength == -1) ?
        [
            theOutput indexOf &int:0 &wide:nl &vint:aLength.
            
            (aLength == -1)
                ? [
                    #var(char:64)aBuffer.
                    #var(int)aBufferLength := 64.
                    
                    $self read &chararray:aBuffer &vint:aBufferLength.
                    (aBufferLength == 0)
                        ? [ aLength := -2. ]  // !! to indicate the eof
                        ! [ theOutput write &chararray:aBuffer &int:aBufferLength. ].
                ].
        ].
        
        (aLength < 0) 
            ? [
                aLine := theOutput wide.                
                theOutput clear.
            ]
            ! [ 
                aLine := theOutput WideSubstring &int:0 &int:aLength.
                theOutput delete &int:0 &int:(aLength + nl_len).
            ].            

        ^ aLine.
    ]
}
