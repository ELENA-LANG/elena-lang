#define system.
#define system'routines.
#define system'text.

// --- BinaryReader ---

#class(limited) BinaryReader
{
    #method read &bytearray:anArray &vlength:anActualLength
    [
    ]

    #method stream [ #throw NotSupportedException new. ]

    #method close [ ]
    
    #method read &bytearray:anArray &length:aLength
    [
        #var(type:length)aRead := $self read &bytearray:anArray &length:aLength.
         
        (aLength > aRead)
            ? [ #throw IOException new. ].
    ]

    #method read &vbyte:anInt
    [
        #var(type:bytearray,size:4)aBuffer.
        #var(type:vlength)aRead := 1.
        
        $self read &bytearray:aBuffer &vlength:aRead.
        
        (aRead < 1)
            ? [ #throw IOException new. ].
            
        aBuffer read &index:0 &vbyte:anInt.
    ]

    #method read &vshort:anInt
    [
        #var(type:bytearray,size:4)aBuffer.
        #var(type:vlength)aRead := 2.
        
        $self read &bytearray:aBuffer &vlength:aRead.
        
        (aRead < 1)
            ? [ #throw IOException new. ].
            
        aBuffer read &index:0 &vshort:anInt.
    ]

    #method read &vchar:anInt
    [
        #var(type:bytearray,size:4)aBuffer.
        #var(type:vlength)aRead := 4.
        
        $self read &bytearray:aBuffer &vlength:aRead.
        
        (aRead < 1)
            ? [ #throw IOException new. ].
            
        aBuffer read &index:0 &vchar:anInt.
    ]

    #method read &vint:anInt
    [
        #var(type:bytearray,size:4)aBuffer.
        #var(type:vlength)aRead := 4.
        
        $self read &bytearray:aBuffer &length:aRead.
        
        (aRead < 4)
            ? [ #throw IOException new. ].
            
        aBuffer read &index:0 &vint:anInt.
    ]

    #method read &vlong:aLong
    [
        #var(type:bytearray,size:8)aBuffer.
        #var(type:vlength)aRead := 8.
        
        $self read &bytearray:aBuffer &vlength:aRead.
        
        (aRead < 8)
            ? [ #throw IOException new. ].
            
        aBuffer read &index:0 &vlong:aLong.
    ]

    #method read &vreal:aReal
    [
        #var(type:bytearray,size:8)aBuffer.
        #var(type:vlength)aRead := 8.
        
        $self read &bytearray:aBuffer &length:8 &vlength:aRead.
        
        (aRead < 8)
            ? [ #throw IOException new. ].
            
        aBuffer read &index:0 &vreal:aReal.
    ]
}

// --- TextReader ---

#class(limited) TextReader
{
    #field(type:textbuffer) theOutput.

    #constructor new
    [
        theOutput := TextBuffer new.
    ]

    #method read &chararray:anArray &vlength:anActualLength
    [
    ]

    #method stream [ #throw NotSupportedException new. ]

    #method encoder [ #throw NotSupportedException new. ]

    #method close [ ]
    
    #method(type:literal) read &literal
    [
        #var(type:literal) aLine := emptyLiteralValue.
        
        #var(type:vint)aLength := -1.
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
                aLine := theOutput literal.                
                theOutput clear.
                
                (aLine is &empty)
                    ? [ ^ nil. ].
            ]
            ! [ 
                aLine := theOutput Substring &index:0 &length:(aLength - 1).
                theOutput delete &index:0 &length:(1 + aLength).
            ].            

        ^ aLine.
    ]
    
    #method(type:wide) read &wide
    [
        #var(type:wide) aLine := emptyWideLiteralValue.
        
        #var(type:vint)aLength := -1.
        #loop (aLength == -1) ?
        [
            theOutput indexOf &index:0 &char:10 &vint:aLength.
            
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
        
        (aLength <= 0) 
            ? [
                aLine := theOutput wide.                
                theOutput clear.
                
                (aLine is &empty)
                    ? [ ^ nil. ].
            ]
            ! [ 
                aLine := theOutput SubWideString &index:0 &length:(aLength - 1).
                theOutput delete &index:0 &length:(1 + aLength).
            ].            

        ^ aLine.
    ]
}
