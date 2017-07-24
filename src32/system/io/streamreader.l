import system'routines.
import system'text.

// --- BinaryReader ---

limited class BinaryReader
{
    stream theStream.
    int    theLength.
    
    constructor new stream:aStream
    [
        theStream := aStream.
        
        theLength := aStream length.
    ]
    
    stacksafe read bytearray:anArray vint:anActualLength
    [
        theStream read bytearray:anArray vint:anActualLength.
    ]

    stream = theStream.

    close => theStream.

    bool available
    [
        int aPosition := theStream index.
        
        ^ aPosition < theLength
    ]
            
    stacksafe read bytearray:anArray int:aLength
    [
        int aRead := aLength.
        
        $self read bytearray:anArray vint:aRead.
         
        if(aLength > aRead)
            [ IOException new; raise ]
    ]

//    read &vbyte:anInt
//    [
//        byte(4) aBuffer := 0.
//        int     aRead := 1.
//        
//        $self read &bytearray:aBuffer &vint:aRead.
//        
//        (aRead < 1)
//            ? [ IOException new raise. ].
//     
//        byte b := aBuffer@0.
//                          
//        anInt := b.
//    ]

    read vshort:anInt
    [
        byte aBuffer(4).
        int  aRead := 2.
        
        $self read bytearray:aBuffer vint:aRead.
        
        if (aRead < 1)
            [ IOException new; raise ].
            
        byteArrayHelper read bytearray:aBuffer int:0 vshort:anInt.
    ]

    read vchar:anInt
    [
        byte aBuffer(4).
        int  aRead := 4.
        
        $self read bytearray:aBuffer vint:aRead.
        
        if (aRead < 1)
            [ IOException new; raise ].
            
        byteArrayHelper read bytearray:aBuffer int:0 vchar:anInt.
    ]

    read vint:anInt
    [
        byte aBuffer(4).
        int  aRead := 4.
        
        $self read bytearray:aBuffer int:aRead.
        
        if (aRead < 4)
            [ IOException new; raise ].
            
        byteArrayHelper read bytearray:aBuffer int:0 vint:anInt.
    ]

    read vlong:aLong
    [
        byte aBuffer(8).
        int  aRead := 8.
        
        $self read bytearray:aBuffer vint:aRead.
        
        if(aRead < 8)
            [ IOException new; raise ].
            
        byteArrayHelper read bytearray:aBuffer int:0 vlong:aLong
    ]

    read vreal:aReal
    [
        byte aBuffer(8).
        int  aRead := 8.
        
        $self read bytearray:aBuffer vint:aRead.
        
        if(aRead < 8)
            [ IOException new; raise ].
            
        byteArrayHelper read bytearray:aBuffer int:0 vreal:aReal
    ]
}

// --- TextReader ---

limited class TextReader
{
    textbuffer theOutput.
    literal    theNewLineConstant.
    int        theNewLineLength.

    constructor new
    [
        theOutput := TextBuilder new.
        theNewLineConstant := 'newLine literal.
                
        theNewLineLength := theNewLineConstant length.
    ]

    stacksafe read chararray:anArray vint:anActualLength
    [
    ]

    stream [ NotSupportedException new; raise ]

    encoder [ NotSupportedException new; raise ]

    close [ ]
    
    bool available = false.
            
    literal read literal
    [
        literal aLine := emptyLiteralValue.
        
        int aLength := -1.
        while (aLength == -1)
        [
            theOutput indexOf int:0 literal:theNewLineConstant vint:aLength.
            
            if(aLength == -1)
                [
                    char aBuffer(64).
                    int   aBufferLength := 64.
                    
                    $self read chararray:aBuffer vint:aBufferLength.
                    if(aBufferLength == 0)
                        [ aLength := -2 ];  // !! to indicate the eof
                        [ theOutput write chararray:aBuffer int:aBufferLength ]
                ]
        ].
        
        if(aLength < 0) 
            [
                aLine := theOutput literal.                
                theOutput clear.
            ];
            [ 
                aLine := theOutput Substring int:0 int:aLength.
                theOutput delete int:0 int:(aLength + theNewLineLength)
            ].

        ^ aLine.
    ]
    
    wide read wide
    [
        wide aLine := emptyWideLiteralValue.
        wide nl := theNewLineConstant wide.
        int  nl_len := 0.
        nl readLengthTo vint:nl_len.
        
        int aLength := -1.
        while (aLength == -1)
        [
            theOutput indexOf int:0 wide:nl vint:aLength.
            
            if(aLength == -1)
                [
                    char aBuffer(64).
                    int  aBufferLength := 64.
                    
                    $self read chararray:aBuffer vint:aBufferLength.
                    if(aBufferLength == 0)
                        [ aLength := -2 ];  // !! to indicate the eof
                        [ theOutput write chararray:aBuffer int:aBufferLength ]
                ]
        ].
        
        if(aLength < 0) 
            [
                aLine := theOutput wide.                
                theOutput clear
            ];
            [ 
                aLine := theOutput WideSubstring int:0 int:aLength.
                theOutput delete int:0 int:(aLength + nl_len)
            ].            

        ^ aLine
    ]
}
