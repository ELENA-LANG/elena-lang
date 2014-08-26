#define system.
#define system'io.

// --- TextReader ---

#class TextReader
{
    #field theStream.
    #field(type:bytearray) theBuffer.
    #field theOutput.
    #field theEncoder.
    #field theOpen.
    
    #constructor new &stream:aStream &encoder:anEncoder
    [
        theStream := aStream.
        theEncoder := anEncoder.
        
        theBuffer := ByteArray new &length:64.
        theOutput := TextBuffer new.
        theOpen := true.

        $self $readFile.        
    ]
    
    #constructor new &path:aPath
        <= %new &stream:(system'io'fileControl openForRead &path:aPath) &encoder:system'io'ansiLiteralEncoder.

    #method $readFile
    [
        theStream read &bytearray:theBuffer &length:64.
        
        #var(type:int)aLength.
        theBuffer read &out'length:aLength.
        
        (0 == aLength)
            ? [ theOpen := false. theStream close. ]
            ! [
                theOutput write &literal::(theEncoder convert &bytearray:theBuffer &length:aLength).
            ].        
    ]

    #method $read &out'int:aRetVal
    [
        $self $readBuffer &out'int:aRetVal.
        
        (-1 == aRetVal)
            ? [ 
                $self $readFile.
            ].
    ]
            
    #method $readBuffer &out'int:aRetVal
        = theOutput indexOf &index:0 &char:10 &out'int:aRetVal.

    #method available = theOpen.

    #method readLine
    [
        #var aLine := emptyLiteralValue.
        #var(type:int)aLength := -1.
        
        theOpen 
            ? [
                #loop (theOpen and:(aLength == -1))
                    ? [ self $read &out'int:aLength. ].
                                
                (0 < aLength)
                    ? [ 
                        aLine := theOutput Substring &index:0 &length:(aLength - 1).
                        
                        theOutput delete &index:0 &length:(1 + aLength).
                    ]
                    ! [
                        theOpen
                            ? [ theOutput delete &index:0 &length:(1 + aLength). ]
                            ! [ aLine := theOutput literal. theOutput clear. ].                
                    ].
            ]
            ! [
                aLine := nil.
            ].
    
        ^ aLine.        
    ]
    
    #method close
    [
        theStream close.
    ]
}
