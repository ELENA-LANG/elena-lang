#define system.

// --- TextReader ---

#class TextReader
{
    #field theStream.
    #field theBuffer.
    #field theOutput.
    #field theEncoder.
    #field theOpen.
    
    #constructor new &stream:aStream &encoder:anEncoder
    [
        theStream := aStream.
        theEncoder := anEncoder.
        
        theBuffer := ByteArray new &type'length:64.
        theOutput := TextBuffer new.
        theOpen := true.

        $self $readFile.        
    ]
    
    #constructor new &path:aPath
        <= new &stream:(system'io'fileControl openForRead &path:aPath) &encoder:system'io'ansiLiteralEncoder.

    #method $readFile
    [
        theStream read &type'bytearray:theBuffer &type'length:64.
        
        (0 == theBuffer type'length)
            ? [ theOpen := false. theStream close. ]
            ! [
                theOutput append &type'widestr:(theEncoder read &type'bytearray:theBuffer &type'length::theBuffer).
            ].        
    ]

    #method $read
    [
        #var anIndex := $self $readBuffer.
        
        (-1 == anIndex)
            ? [ 
                $self $readFile.
            ].
            
        ^ anIndex.
    ]
            
    #method $readBuffer
        = theOutput indexOf &type'index:0 &type'widechar:10.

    #method EOF = theOpen inverted.

    #method readLine
    [
        #var aLine := emptyLiteralValue.
        #var aLength := -1.
        
        theOpen 
            ? [
                control while:[ theOpen and:(aLength == -1) ] &do:[ aLength := self $read. ].
                                
                (0 < aLength)
                    ? [ 
                        aLine := theOutput read &type'index:0 &type'length:(aLength + 1).
                        
                        system'external'CoreAPI _trimLine &out'type'widestr:aLine.
                        
                        theOutput delete &type'index:0 &type'length:(1 + aLength).
                    ]
                    ! [
                        theOpen
                            ? [ theOutput delete &type'index:0 &type'length:(1 + aLength). ]
                            ! [ aLine := theOutput Literal. theOutput clear. ].                
                    ].
//                
//                ^ aLine.
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
