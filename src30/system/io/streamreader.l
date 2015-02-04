#define system.
#define system'routines.
#define system'text.

// --- BinaryReader ---

#class(limited) BinaryReader
{
    #method read &bytearray:anArray &length:aMaxLength &out'length:anActualLength
    [
    ]

    #method source = nil.

    #method close [ ]
    
    #method read &bytearray:anArray &length:aLength
    [
        #var(type:length)aRead := $self read &bytearray:anArray &length:aLength.
         
        (aLength > aRead)
            ? [ #throw IOException new. ].
    ]

    #method read &out'int:anInt
    [
        #var(type:bytearray,size:4)aBuffer.
        #var(type:length)aRead := 4.
        
        aRead := $self read &bytearray:aBuffer &length:4.
        
        (aRead < 4)
            ? [ #throw IOException new. ].
            
        anInt := aBuffer read &index:0.
    ]

    #method read &out'long:aLong
    [
        #var(type:bytearray,size:8)aBuffer.
        #var(type:length)aRead := 8.
        
        aRead := $self read &bytearray:aBuffer &length:8.
        
        (aRead < 8)
            ? [ #throw IOException new. ].
            
        aLong := aBuffer read &index:0.
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

    #method read &shortarray:anArray &length:aMaxLength &out'length:anActualLength
    [
    ]

    #method source = nil.

    #method encoder [ #throw NotSupportedException new. ]

    #method close [ ]
    
    #method readLine
    [
        #var(type:literal) aLine := emptyLiteralValue.
        
        #var(type:int)aLength := -1.
        #loop (aLength == -1) ?
        [
            aLength := theOutput indexOf &index:0 &short:10.
            
            (aLength == -1)
                ? [
                    #var(type:shortarray,size:64)aBuffer.
                    #var(type:length)aBufferLength := 64.
                    
                    aBufferLength := $self read &shortarray:aBuffer &length:aBufferLength.
                    (aBufferLength == 0)
                        ? [ aLength := -2. ]  // !! to indicate the eof
                        ! [ theOutput write &shortarray:aBuffer &length:aBufferLength. ].
                ].
        ].
        
        (aLength <= 0) 
            ? [
                aLine := theOutput literal.                
                theOutput clear.
                
                (aLine isEmpty)
                    ? [ ^ nil. ].
            ]
            ! [ 
                aLine := theOutput Substring &index:0 &length:(aLength - 1).
                theOutput delete &index:0 &length:(1 + aLength).
            ].            

        ^ aLine.
    ]
}
