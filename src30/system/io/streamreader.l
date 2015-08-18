#import system.
#import system'routines.
#import system'text.

// --- BinaryReader ---

#class(limited) BinaryReader
{
    #field(type:stream)theStream.
    #field(type:int)theLength.
    
    #constructor new &stream:aStream
    [
        theStream := aStream.
        
        theLength := aStream length int.
    ]
    
    #method(stacksafe) read &bytearray:anArray &vint:anActualLength
    [
        theStream read &bytearray:anArray &vint:anActualLength.
    ]

    #method stream = theStream.

    #method close => theStream.

    #method(type:bool) available
    [
        #var(type:int)aPosition.
        
        theStream readIndex &vint:aPosition.
        
        ^ aPosition < theLength.
    ]
            
    #method(stacksafe) read &bytearray:anArray &int:aLength
    [
        #var(type:int)aRead := aLength.
        
        $self read &bytearray:anArray &vint:aRead.
         
        (aLength > aRead)
            ? [ #throw IOException new. ].
    ]

    #method read &vbyte:anInt
    [
        #var(type:bytearray,size:4)aBuffer.
        #var(type:int)aRead := 1.
        
        $self read &bytearray:aBuffer &vint:aRead.
        
        (aRead < 1)
            ? [ #throw IOException new. ].
            
        aBuffer read &int:0 &vbyte:anInt.
    ]

    #method read &vshort:anInt
    [
        #var(type:bytearray,size:4)aBuffer.
        #var(type:int)aRead := 2.
        
        $self read &bytearray:aBuffer &vint:aRead.
        
        (aRead < 1)
            ? [ #throw IOException new. ].
            
        aBuffer read &int:0 &vshort:anInt.
    ]

    #method read &vchar:anInt
    [
        #var(type:bytearray,size:4)aBuffer.
        #var(type:int)aRead := 4.
        
        $self read &bytearray:aBuffer &vint:aRead.
        
        (aRead < 1)
            ? [ #throw IOException new. ].
            
        aBuffer read &int:0 &vchar:anInt.
    ]

    #method read &vint:anInt
    [
        #var(type:bytearray,size:4)aBuffer.
        #var(type:int)aRead := 4.
        
        $self read &bytearray:aBuffer &int:aRead.
        
        (aRead < 4)
            ? [ #throw IOException new. ].
            
        aBuffer read &int:0 &vint:anInt.
    ]

    #method read &vlong:aLong
    [
        #var(type:bytearray,size:8)aBuffer.
        #var(type:int)aRead := 8.
        
        $self read &bytearray:aBuffer &vint:aRead.
        
        (aRead < 8)
            ? [ #throw IOException new. ].
            
        aBuffer read &int:0 &vlong:aLong.
    ]

    #method read &vreal:aReal
    [
        #var(type:bytearray,size:8)aBuffer.
        #var(type:int)aRead := 8.
        
        $self read &bytearray:aBuffer &vint:aRead.
        
        (aRead < 8)
            ? [ #throw IOException new. ].
            
        aBuffer read &int:0 &vreal:aReal.
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
                
        theNewLineLength := theNewLineConstant length int.
    ]

    #method(stacksafe) read &chararray:anArray &vint:anActualLength
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
            theOutput indexOf &int:0 &literal:theNewLineConstant &vint:aLength.
            
            (aLength == -1)
                ? [
                    #var(type:chararray,size:64)aBuffer.
                    #var(type:int)aBufferLength := 64.
                    
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
    
    #method(type:wide) read &wide
    [
        #var(type:wide) aLine := emptyWideLiteralValue.
        #var(type:wide) nl := theNewLineConstant wide.
        #var(type:int) nl_len.
        nl readLength &vint:nl_len.
        
        #var(type:int)aLength := -1.
        #loop (aLength == -1) ?
        [
            theOutput indexOf &int:0 &wide:nl &vint:aLength.
            
            (aLength == -1)
                ? [
                    #var(type:chararray,size:64)aBuffer.
                    #var(type:int)aBufferLength := 64.
                    
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
