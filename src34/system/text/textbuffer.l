const int TextPageSize = 128.

// --- TextBuffer ---

sealed public /*nonstructural */ class TextBuilder
{
    chararray theBuffer.
    ref<int>  theLength.

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

    stacksafe readLengthTo(ref<int> aRetVal)
    [
        aRetVal value := theLength.
    ]

    stacksafe setLength(int aLength)
    [
        theLength value := aLength.
    ]

    set length:aLength
        <= setLength(T<int>(aLength)).

    cast T<chararray> = theBuffer.

    T<literal> literal
        = LiteralValue new(0, T<int>(theLength), theBuffer).

//    wide
//        = WideLiteralValue new int:0 int:theLength chararray:theBuffer.

    writeLine
    [
        self write:forwards'newLine.
    ]

    append : aLine
    [
        self write:aLine.
    ]
                                    
    stacksafe reserve(int aLength)
    [
        int aNewLength := aLength.
        
        aNewLength := aNewLength + theLength.
        
        int aBufferLength := theBuffer length.
        
        if (aNewLength >= aBufferLength)
        [
            //aNewLength := (aNewLength / $TextPageSize + 1) * $TextPageSize.
            aNewLength := aNewLength / TextPageSize.
            aNewLength := aNewLength + 1.
            aNewLength := aNewLength * TextPageSize.            
            
            chararray aNewBuffer := CharArray new(aNewLength).
            
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
            
            aLiteral save(theLength, theBuffer, &aLength).
            
            theLength append(aLength).
        ].
    ]

    write(wide aLiteral)
    [
        int aLength := aLiteral length.
        
        ifnot (0 == aLength)
        [
            self reserve(aLength).
            
            aLiteral save(theLength, theBuffer, &aLength).
            
            theLength append(aLength).
        ].
    ]

    stacksafe write(char aChar)
    [
        self reserve(1).
        
        theBuffer write(T<int>(theLength), aChar).
        
        theLength append(1).
    ]

    stacksafe write(int n)
    [
        T<byte,16> tmp.
        T<char,16> ch_tmp.
        int        len := 16.
        
        literalConvertor convert(n, T<bytearray>(tmp), &len).
        
        UTF8Encoding convert(0, &len, T<bytearray>(tmp), T<chararray>(ch_tmp), &len).
        
        self write(T<chararray>(ch_tmp), len).
    ]

    write : anObject
    [
        self write(anObject literal).
    ]

    stacksafe write(chararray anArray, int aLength)
    [
        ifnot (0 == aLength)
        [
            self reserve(aLength).
            
            theBuffer write(theLength, aLength, anArray).
            
            theLength append(aLength).
        ].
    ]

    private stacksafe readIndex(literal s, ref<int> i, ref<int> retVal)
    [
        char ch := $0.
        int  n := 0.
        int  v := 0.
        
        s read(i, &ch).
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
            s read(i, &ch)
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

    stacksafe fill(literal s, int counter)
    [
        int i := 0.
        while (i < counter)
        [
            self write(s).
        ]
    ]

    stacksafe fill(wide s, int counter)
    [
        int i := 0.
        while (i < counter)
        [
            self write(s).
        ]
    ]

    stacksafe fill(char ch, int counter)
    [
        int i := 0.
        while (i < counter)
        [
            self write(ch).
            
            i += 1
        ]
    ]

    stacksafe insert(int anIndex, int aLength, chararray anArray)
    [
        self reserve(aLength).
        
        theBuffer move(anIndex, theLength, aLength).
        
        theBuffer write(anIndex, aLength, anArray).
        
        theLength append(aLength).
    ]

    stacksafe insert(int anIndex, literal aLiteral)
    [
        int aLength := 0.
        UTF8Encoding charCount(aLiteral, &aLength).

        self reserve(aLength).
        
        theBuffer move(anIndex, theLength, aLength).
        
        aLiteral save(anIndex, theBuffer, &aLength).
        
        theLength append(aLength).
    ]

    stacksafe insert(int anIndex, wide aLiteral)
    [
        int aLength := 0.
        UTF16Encoding charCount(aLiteral, &aLength).

        self reserve(aLength).
        
        theBuffer move(anIndex, theLength, aLength).
        
        aLiteral save(anIndex, theBuffer, &aLength).
        
        theLength append(aLength).
    ]
    
    stacksafe insert(int anIndex, char aChar)
    [
        self reserve(1).
        
        theBuffer move(anIndex, theLength, 1).
        
        theBuffer write(anIndex, aChar).
        
        theLength append(1).
    ]

    insert : anObject at:anIndex
        <= insert(T<int>(anIndex), anObject).

    stacksafe indexOf(int aPosition, char aValue, ref<int> aRetVal)
    [
        int  anIndex := 0.
        char aCurrent := $0.
        
        while (anIndex < theLength)
        [
            theBuffer read(anIndex, &aCurrent).
            
            if (aCurrent == aValue)
                [ aRetVal value := anIndex. ^self ].
                
            anIndex += 1
        ].
        
        aRetVal value := -1
    ]

    stacksafe indexOf(int aPosition, literal aLiteral, ref<int> aRetVal)
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

    stacksafe indexOf(int aPosition, wide aLiteral, ref<int> aRetVal)
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

    stacksafe literal Substring(int anIndex, int aLength)
    [
        if (aLength == 0)
            [ ^ emptyLiteralValue ].
            
        ^ LiteralValue new(anIndex, aLength, theBuffer)
    ]

    stacksafe wide WideSubstring(int anIndex, int aLength)
    [
        if (aLength == 0)
            [ ^ emptyWideLiteralValue ].
            
        ^ WideLiteralValue new(anIndex, aLength, theBuffer)
    ]

    stacksafe delete(int anIndex, int aLength)
    [
        int n := 0 - aLength.
        int l := theLength.
        l := l - anIndex.
        l := l - aLength.
        
        theBuffer move(anIndex + aLength, l, n).
        
        theLength reduce(aLength)
    ]
    
    stacksafe trim(int n)
    [
        theLength reduce(n)
    ]
}
