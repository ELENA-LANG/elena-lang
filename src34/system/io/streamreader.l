//import system'routines.
import system'text.

// --- BinaryReader ---

public interface BinaryReader
{
//    stream theStream.
//    int    theLength.
//    
//    constructor new stream:aStream
//    [
//        theStream := aStream.
//        
//        theLength := aStream length.
//    ]
//    
//    stacksafe read bytearray:anArray vint:anActualLength
//    [
//        theStream read bytearray:anArray vint:anActualLength.
//    ]
//
//    stream = theStream.
//
//    close => theStream.
//
//    bool available
//    [
//        int aPosition := theStream index.
//        
//        ^ aPosition < theLength
//    ]
//            
//    stacksafe read bytearray:anArray int:aLength
//    [
//        int aRead := aLength.
//        
//        $self read bytearray:anArray vint:aRead.
//         
//        if(aLength > aRead)
//            [ IOException new; raise ]
//    ]
//
//    read vbyte:anInt
//    [
//        byte<4> aBuffer.
//        int     aRead := 1.
//        
//        $self read bytearray:aBuffer vint:aRead.
//        
//        if (aRead < 1)
//            [ IOException new; raise ].
//     
//        byte b := aBuffer[0].
//                          
//        anInt byte := b.
//    ]
//
//    read vshort:anInt
//    [
//        byte<4> aBuffer.
//        int     aRead := 2.
//        
//        $self read bytearray:aBuffer vint:aRead.
//        
//        if (aRead < 1)
//            [ IOException new; raise ].
//            
//        byteArrayHelper read bytearray:aBuffer int:0 vshort:anInt.
//    ]
//
//    read vchar:anInt
//    [
//        byte<4> aBuffer.
//        int     aRead := 4.
//        
//        $self read bytearray:aBuffer vint:aRead.
//        
//        if (aRead < 1)
//            [ IOException new; raise ].
//            
//        byteArrayHelper read bytearray:aBuffer int:0 vchar:anInt.
//    ]
//
//    read vint:anInt
//    [
//        byte<4> aBuffer.
//        int     aRead := 4.
//        
//        $self read bytearray:aBuffer int:aRead.
//        
//        if (aRead < 4)
//            [ IOException new; raise ].
//            
//        byteArrayHelper read bytearray:aBuffer int:0 vint:anInt.
//    ]
//
//    read vlong:aLong
//    [
//        byte<8> aBuffer.
//        int     aRead := 8.
//        
//        $self read bytearray:aBuffer vint:aRead.
//        
//        if(aRead < 8)
//            [ IOException new; raise ].
//            
//        byteArrayHelper read bytearray:aBuffer int:0 vlong:aLong
//    ]
//
//    read vreal:aReal
//    [
//        byte<8> aBuffer.
//        int     aRead := 8.
//        
//        $self read bytearray:aBuffer vint:aRead.
//        
//        if(aRead < 8)
//            [ IOException new; raise ].
//            
//        byteArrayHelper read bytearray:aBuffer int:0 vreal:aReal
//    ]
}

// --- TextReader ---

public interface TextReader
{
    T<TextBuilder> theOutput.
    literal        theNewLineConstant.
    int            theNewLineLength.

    constructor new
    [
        theOutput := TextBuilder new.
        theNewLineConstant := forwards'newLine.
                
        theNewLineLength := theNewLineConstant length.
    ]

    abstract stacksafe read(chararray anArray, ref<int> anActualLength) []

//    stream [ NotSupportedException new; raise ]
//
//    encoder [ NotSupportedException new; raise ]

    abstract close [ ]
    
    abstract bool available []
            
    literal readLiteral
    [
        literal aLine := emptyLiteralValue.
        
        int aLength := -1.
        while (aLength == -1)
        [
            theOutput indexOf(0, theNewLineConstant, &aLength).
            
            if(aLength == -1)
                [
                    T<char,64> aBuffer.
                    int        aBufferLength := 64.
                    
                    self read(T<chararray>(aBuffer), &aBufferLength).
                    if(aBufferLength == 0)
                        [ aLength := -2 ];  // !! to indicate the eof
                        [ theOutput write(T<chararray>(aBuffer), aBufferLength) ]
                ]
        ].
        
        if(aLength < 0) 
            [
                aLine := T<literal>(theOutput).                
                theOutput clear.
            ];
            [ 
                aLine := theOutput Substring(0, aLength).
                theOutput delete(0, aLength + theNewLineLength)
            ].

        ^ aLine.
    ]
    
    wide readWide
    [
        wide aLine := emptyWideLiteralValue.
        wide nl := T<wide>(theNewLineConstant).
        int  nl_len := 0.
        nl readLengthTo(&nl_len).
        
        int aLength := -1.
        while (aLength == -1)
        [
            theOutput indexOf(0, nl, &aLength).
            
            if(aLength == -1)
                [
                    T<char,64> aBuffer.
                    int        aBufferLength := 64.
                     
                    self read(T<chararray>(aBuffer), &aBufferLength).
                    if(aBufferLength == 0)
                        [ aLength := -2 ];  // !! to indicate the eof
                        [ theOutput write(T<chararray>(aBuffer), aBufferLength) ]
                ]
        ].
        
        if(aLength < 0) 
            [
                aLine := T<wide>(theOutput).
                theOutput clear
            ];
            [ 
                aLine := theOutput WideSubstring(0, aLength).
                theOutput delete(0, aLength + nl_len)
            ].            

        ^ aLine
    ]
}
