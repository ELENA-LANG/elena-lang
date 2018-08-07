//import system'routines.
import system'text.

// --- BinaryReader ---

public interface BinaryReader
{
    T<Stream> theStream.
    int       theLength.
    
    stacksafe read(ByteArray anArray, ref<int> anActualLength)
    [
        theStream read(anArray, anActualLength).
    ]

    T<Stream> stream = theStream.

    close => theStream.

    bool available
    [
        int aPosition := theStream index.
        
        ^ aPosition < theLength
    ]
            
    stacksafe read(ByteArray anArray, int aLength)
    [
        int aRead := aLength.
        
        self read(anArray, &aRead).
         
        if(aLength > aRead)
            [ IOException new; raise ]
    ]

    read(ref<byte> anInt)
    [
        V<byte,4> aBuffer.
        int       aRead := 1.
        
        self read(T<ByteArray>(aBuffer), &aRead).
        
        if (aRead < 1)
            [ IOException new; raise ].
     
        byte b := aBuffer[0].
                          
        anInt byte := b.
    ]

    read(ref<short> anInt)
    [
        V<byte,4> aBuffer.
        int       aRead := 2.
        
        self read(T<ByteArray>(aBuffer), &aRead).
        
        if (aRead < 1)
            [ IOException new; raise ].
            
        byteArrayHelper read(T<ByteArray>(aBuffer), 0, anInt).
    ]

    read(ref<char> anInt)
    [
        V<byte,4> aBuffer.
        int       aRead := 4.
        
        self read(T<ByteArray>(aBuffer), &aRead).
        
        if (aRead < 1)
            [ IOException new; raise ].
            
        byteArrayHelper read(T<ByteArray>(aBuffer), 0, anInt).
    ]

    read(ref<int> anInt)
    [
        V<byte,4> aBuffer.
        int       aRead := 4.
        
        self read(T<ByteArray>(aBuffer), &aRead).
        
        if (aRead < 4)
            [ IOException new; raise ].
            
        byteArrayHelper read(T<ByteArray>(aBuffer), 0, anInt).
    ]

    read(ref<long> aLong)
    [
        V<byte,8> aBuffer.
        int       aRead := 8.
        
        self read(T<ByteArray>(aBuffer), &aRead).
        
        if(aRead < 8)
            [ IOException new; raise ].
            
        byteArrayHelper read(T<ByteArray>(aBuffer), 0, aLong)
    ]

    read(ref<real> aReal)
    [
        V<byte,8> aBuffer.
        int       aRead := 8.
        
        self read(T<ByteArray>(aBuffer), &aRead).
        
        if(aRead < 8)
            [ IOException new; raise ].
            
        byteArrayHelper read(T<ByteArray>(aBuffer), 0, aReal)
    ]
}

// --- TextReader ---

public interface TextReader
{
    T<TextBuilder> theOutput.
    literal        theNewLineConstant.
    int            theNewLineLength.

    abstract stacksafe read(CharArray anArray, ref<int> anActualLength) []

    abstract T<Stream> stream [ ]

    abstract T<Encoder> encoder [ ]

    abstract close [ ]
    
    abstract bool available []
            
    literal readLiteral
    [
        literal aLine := emptyLiteral.
        
        int aLength := -1.
        while (aLength == -1)
        [
            theOutput indexOf(0, theNewLineConstant, &aLength).
            
            if(aLength == -1)
                [
                    T<char,64> aBuffer.
                    int        aBufferLength := 64.
                    
                    self read(T<CharArray>(aBuffer), &aBufferLength).
                    if(aBufferLength == 0)
                        [ aLength := -2 ];  // !! to indicate the eof
                        [ theOutput write(T<CharArray>(aBuffer), aBufferLength) ]
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
        wide aLine := emptyWideLiteral.
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
                     
                    self read(T<CharArray>(aBuffer), &aBufferLength).
                    if(aBufferLength == 0)
                        [ aLength := -2 ];  // !! to indicate the eof
                        [ theOutput write(T<CharArray>(aBuffer), aBufferLength) ]
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
