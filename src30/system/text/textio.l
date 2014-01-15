#define system.
#define system'io.

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
        
        theBuffer := ByteArray new &length:64.
        theOutput := TextBuffer new.
        theOpen := true.

        $self $readFile.        
    ]
    
    #constructor new &path:aPath
        => &new &stream:(system'io'fileControl openForRead &path:aPath) &encoder:system'io'ansiLiteralEncoder.

    #method $readFile
    [
        theStream read &bytearray:theBuffer &length:64.
        
        (0 == theBuffer length)
            ? [ theOpen := false. theStream close. ]
            ! [
                theOutput write &literal:(theEncoder read &bytearray:theBuffer &length::theBuffer).
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
        = theOutput indexOf &index:0 &char:10.

    #method available = theOpen.

    #method readLine
    [
        #var aLine := emptyLiteralValue.
        #var aLength := -1.
        
        theOpen 
            ? [
                control while:[ theOpen and:(aLength == -1) ] &do:[ aLength := self $read. ].
                                
                (0 < aLength)
                    ? [ 
                        aLine := theOutput Substring &index:0 &length:(aLength + 1).
                        
                        consoleHelper _trimLine &literal:aLine.
                        
                        theOutput delete &index:0 &length:(1 + aLength).
                    ]
                    ! [
                        theOpen
                            ? [ theOutput delete &index:0 &length:(1 + aLength). ]
                            ! [ aLine := theOutput literal. theOutput clear. ].                
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
