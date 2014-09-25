#define system.
#define system'routines.

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

// --- BinaryReader

#class BinaryReader
{
}
