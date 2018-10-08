const int TextPageSize = 128.

// --- TextBuffer ---

sealed public /*nonstructural */ class TextBuilder
{
    T<CharArray> theBuffer.
    ref<int>     theLength.

    constructor new
    [
        theBuffer := CharArray new(TextPageSize).
        theLength := 0.
    ]

    constructor new : value
        <= new;
    [
        self write(value).
    ] 

    int rvprop length :: readLengthTo.

    clear
    [
        theLength value := 0.
    ]

    readLengthTo(ref<int> aRetVal)
    [
        theLength readValueTo(aRetVal).
    ]

    setLength(int aLength)
    [
        theLength value := aLength.
    ]

    set length:aLength
        <= setLength(T<int>(aLength)).

    cast T<CharArray> = theBuffer.

    cast T<literal> 
        = LiteralValue new(0, T<int>(theLength), theBuffer).

    T<literal> literal
        = LiteralValue new(0, T<int>(theLength), theBuffer).

    cast T<wide>
        = WideLiteralValue new(0, T<int>(theLength), theBuffer).

    writeLine
    [
        self write:forwards'newLine.
    ]

    append : aLine
    [
        self write:aLine.
    ]
                                    
    reserve(int aLength)
    [
        int aNewLength := aLength.
        
        aNewLength := aNewLength + T<int>(theLength).
        
        int aBufferLength := theBuffer length.
        
        if (aNewLength >= aBufferLength)
        [
            //aNewLength := (aNewLength / $TextPageSize + 1) * $TextPageSize.
            aNewLength := aNewLength / TextPageSize.
            aNewLength := aNewLength + 1.
            aNewLength := aNewLength * TextPageSize.            
            
            T<CharArray> aNewBuffer := CharArray new(aNewLength).
            
            aNewBuffer write(0, aBufferLength, theBuffer).
            
            theBuffer := aNewBuffer.
        ].
    ]

    write(literal aLiteral)
    [
        int aLength := aLiteral length.
        
        ifnot (0 == aLength)
        [
            self reserve(aLength).
            
            aLiteral save(T<int>(theLength), theBuffer, &aLength).
            
            theLength append(aLength).
        ].
    ]

    write(wide aLiteral)
    [
        int aLength := aLiteral length.
        
        ifnot (0 == aLength)
        [
            self reserve(aLength).
            
            aLiteral save(T<int>(theLength), theBuffer, &aLength).
            
            theLength append(aLength).
        ].
    ]

    write(char aChar)
    [
        self reserve(1).
        
        theBuffer write(T<int>(theLength), aChar).
        
        theLength append(1).
    ]

    write(int n)
    [
        T<byte,16> tmp.
        T<char,16> ch_tmp.
        int        len := 16.
        
        literalConvertor convert(n, T<ByteArray>(tmp), &len).
        
        UTF8Encoding convert(0, &len, T<ByteArray>(tmp), T<CharArray>(ch_tmp), &len).
        
        self write(T<CharArray>(ch_tmp), len).
    ]

    write : anObject
    [
        self write(anObject literal).
    ]

    write(CharArray anArray, int aLength)
    [
        ifnot (0 == aLength)
        [
            self reserve(aLength).
            
            theBuffer write(T<int>(theLength), aLength, anArray).
            
            theLength append(aLength).
        ].
    ]

    private readIndex(literal s, ref<int> i, ref<int> retVal)
    [
        char ch := $0.
        int  n := 0.
        int  v := 0.
        
        int i_val := i.
        
        s read(i_val, &ch).
        while (ch != $125)
        [
            intConvertor convert(ch, &n).
            if((n >= 30h)and:(n <= 39h))
            [                
                n := n - 30h.
                
                v := v * 10.
                v := v + n.
            ];
            [ InvalidArgumentException new; raise ].
                
            i append(1).
            i_val := i.
            s read(i_val, &ch)
        ].
        
        retVal value := v
    ]

    generic writeFormatted(literal s, V<object> args)
    [
        int  len := s length.
        int  i := 0.
        char ch := $0.
        int  chlen := 0.
        int  index := 0.
        
        while (i < len)
        [
            s read(i, &ch).
            
            if (ch == $123)
                [ 
                    i := i + 1.
                    
                    self readIndex(s, &i, &index).
                    self write(args[index]).
                    
                    chlen := 1.
                ];
                [ 
                    self write(ch).
                    ch readLengthTo(&chlen)
                ].
            
            i := i + chlen
        ]
    ]

    fill(literal s, int counter)
    [
        int i := 0.
        while (i < counter)
        [
            self write(s).
        ]
    ]

    fill(wide s, int counter)
    [
        int i := 0.
        while (i < counter)
        [
            self write(s).
        ]
    ]

    fill(char ch, int counter)
    [
        int i := 0.
        while (i < counter)
        [
            self write(ch).
            
            i += 1
        ]
    ]

    insert(int anIndex, int aLength, CharArray anArray)
    [
        self reserve(aLength).
        
        theBuffer move(anIndex, T<int>(theLength), aLength).
        
        theBuffer write(anIndex, aLength, anArray).
        
        theLength append(aLength).
    ]

    insert(int anIndex, literal aLiteral)
    [
        int aLength := 0.
        UTF8Encoding charCount(aLiteral, &aLength).

        self reserve(aLength).
        
        theBuffer move(anIndex, T<int>(theLength), aLength).
        
        aLiteral save(anIndex, theBuffer, &aLength).
        
        theLength append(aLength).
    ]

    insert(int anIndex, wide aLiteral)
    [
        int aLength := 0.
        UTF16Encoding charCount(aLiteral, &aLength).

        self reserve(aLength).
        
        theBuffer move(anIndex, T<int>(theLength), aLength).
        
        aLiteral save(anIndex, theBuffer, &aLength).
        
        theLength append(aLength).
    ]
    
    insert(int anIndex, char aChar)
    [
        self reserve(1).
        
        theBuffer move(anIndex, T<int>(theLength), 1).
        
        theBuffer write(anIndex, aChar).
        
        theLength append(1).
    ]

    insert : anObject at:anIndex
        <= insert(T<int>(anIndex), anObject).

    indexOf(int aPosition, char aValue, ref<int> aRetVal)
    [
        int  anIndex := 0.
        char aCurrent := $0.
        
        while (anIndex < T<int>(theLength))
        [
            theBuffer read(anIndex, &aCurrent).
            
            if (aCurrent == aValue)
                [ aRetVal value := anIndex. ^self ].
                
            anIndex += 1
        ].
        
        aRetVal value := -1
    ]

    indexOf(int aPosition, literal aLiteral, ref<int> aRetVal)
    [
        int  aLiteralLength := 0.
        int  anIndex := 0.
        int  aFirstCharLen := 0.
        char aCurrent := $0.
        char aFirstChar := $0.
        char aNextChar := $0.
        
        aLiteral readLengthTo(&aLiteralLength).
        
        aLiteral read(0, &aFirstChar).
        aFirstChar readLengthTo(&aFirstCharLen).
        
        int aMaxIndex := theLength.
        aMaxIndex := aMaxIndex - aLiteralLength.
        
        while (anIndex < aMaxIndex)
        [
            theBuffer read(anIndex, &aCurrent).
            
            if (aCurrent == aFirstChar)
            [ 
                int j := aFirstCharLen.
                int chLen := 0.
                while (j < aLiteralLength)
                [
                    theBuffer read(anIndex + j, &aCurrent).
                    aLiteral read(j, &aNextChar).
                    
                    if (aNextChar == aCurrent)
                        [ aNextChar readLengthTo(&chLen) ];
                        [ j := aLiteralLength. chLen := 1 ].
                    
                    j := j + chLen
                ].
                
                if (j == aLiteralLength)
                    [ aRetVal value := anIndex. ^ self ]
            ].
                
            anIndex := anIndex + 1
        ].
        
        aRetVal value := -1
    ]

    indexOf(int aPosition, wide aLiteral, ref<int> aRetVal)
    [
        int  aLiteralLength := 0.
        int  anIndex := 0.
        int  aFirstCharLen := 0.
        char aCurrent := $0.
        char aFirstChar := $0.
        char aNextChar := $0.
        
        aLiteral readLengthTo(&aLiteralLength).
        
        aLiteral read(0, &aFirstChar).
        aFirstChar readWideLengthTo(&aFirstCharLen).
        
        while (anIndex < theLength)
        [
            theBuffer read(anIndex, &aCurrent).
            
            if (aCurrent == aFirstChar)
            [ 
                int j := aFirstCharLen.
                int chLen := 0.
                while (j < aLiteralLength)
                [
                    theBuffer read(anIndex + j, &aCurrent).
                    aLiteral read(j, &aNextChar).
                    
                    if (aNextChar == aCurrent)
                        [ aNextChar readWideLengthTo(&chLen) ];
                        [ j := aLiteralLength. chLen := 1 ].
                    
                    j := j + chLen
                ].
                
                if (j > aLiteralLength)
                    [ aRetVal value := anIndex. ^ self ]
            ].
                
            anIndex := anIndex + 1
        ].
        
        aRetVal value := -1
    ]

    indexOf(int aPosition, char aValue)
    [
        int aRetVal := 0.
        self indexOf(aPosition, aValue, &aRetVal).
        
        ^ IntNumber new(aRetVal)
    ]
    
    indexOf(int aPosition, literal aValue)
    [
        int aRetVal := 0.
        self indexOf(aPosition, aValue, &aRetVal).
        
        ^ IntNumber new(aRetVal)
    ]
    
    indexOf(int aPosition, wide aValue)
    [
        int aRetVal := 0.
        self indexOf(aPosition, aValue, &aRetVal).
        
        ^ IntNumber new(aRetVal)
    ]

    literal Substring(int anIndex, int aLength)
    [
        if (aLength == 0)
            [ ^ emptyLiteral ].
            
        ^ LiteralValue new(anIndex, aLength, theBuffer)
    ]

    wide WideSubstring(int anIndex, int aLength)
    [
        if (aLength == 0)
            [ ^ emptyWideLiteral ].
            
        ^ WideLiteralValue new(anIndex, aLength, theBuffer)
    ]

    delete(int anIndex, int aLength)
    [
        int n := 0 - aLength.
        int l := theLength.
        l := l - anIndex.
        l := l - aLength.
        
        theBuffer move(anIndex + aLength, l, n).
        
        theLength reduce(aLength)
    ]
    
    trim(int n)
    [
        theLength reduce(n)
    ]
}
