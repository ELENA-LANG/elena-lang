const int TextPageSize = 128.

// --- TextBuffer ---

sealed public /*nonstructural */ class TextBuilder
{
    chararray theBuffer.
    ref<int>  theLength.

    constructor new
    [
        theBuffer := CharArray new(TextPageSize).
        theLength value := 0.
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
        = LiteralValue new(0, theLength, theBuffer).

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
        
        theBuffer write(theLength, aChar).
        
        theLength append(1).
    ]

//    stacksafe write(int n)
//    [
//        byte<16> tmp.
//        char<16> ch_tmp.
//        int      len := 16.
//        
//        literalConvertor convert int:n bytearray:tmp vint:len.
//        
//        UTF8Encoding convert int:0 vint:len bytearray:tmp chararray:ch_tmp vint:len.
//        
//        $self write chararray:ch_tmp int:len.
//    ]

//    multi write : anObject
//    [
//        $self write literal:anObject.
//    ]
//
//    stacksafe write chararray:anArray int:aLength
//    [
//        ifnot (0 == aLength)
//        [
//            $self reserve int:aLength.
//            
//            theBuffer write int:theLength int:aLength chararray:anArray.
//            
//            theLength append int:aLength.
//        ].
//    ]
//
//    sealed stacksafe $readIndex literal:s vint:i vint:retVal
//    [
//        char ch := $0.
//        int  n := 0.
//        int  v := 0.
//        
//        s read int:i vchar:ch.
//        while (ch != $125)
//        [
//            intConvertor convert char:ch vint:n.
//            if((n >= 30h)and:(n <= 39h))
//            [                
//                n := n - 30h.
//                
//                v := v * 10.
//                v := v + n.
//            ];
//            [ InvalidArgumentException new; raise ].
//                
//            i append int:1.
//            s read int:i vchar:ch
//        ].
//        
//        retVal int := v
//    ]
//
//    generic writeFormatted(literal s, object<> args)
//    [
//        int  len := s length.
//        int  i := 0.
//        char ch := $0.
//        int  chlen := 0.
//        int  index := 0.
//        
//        while (i < len)
//        [
//            s read int:i vchar:ch.
//            
//            if (ch == $123)
//                [ 
//                    i := i + 1.
//                    
//                    $self $readIndex literal:s vint:i vint:index.
//                    $self write(args[index]).
//                    
//                    chlen := 1.
//                ];
//                [ 
//                    $self write char:ch. 
//                    ch readLengthTo vint:chlen
//                ].
//            
//            i := i + chlen
//        ]
//    ]
//
//    fill(literal s, int counter)
//    [
//        int i := 0.
//        while (i < counter)
//        [
//            $self write(s).
//        ]
//    ]
//
//    fill(wide s, int counter)
//    [
//        int i := 0.
//        while (i < counter)
//        [
//            $self write(s).
//        ]
//    ]
//
//    fill(char ch, int counter)
//    [
//        int i := 0.
//        while (i < counter)
//        [
//            $self write(ch).
//            
//            i += 1
//        ]
//    ]
//
//    insert int:anIndex int:aLength chararray:anArray
//    [
//        $self reserve int:aLength.
//        
//        theBuffer move int:anIndex int:theLength int:aLength.
//        
//        theBuffer write int:anIndex int:aLength chararray:anArray.
//        
//        theLength append int:aLength.
//    ]
//
//    insert int:anIndex literal:aLiteral
//    [
//        int aLength := 0.
//        UTF8Encoding charCount literal:aLiteral vint:aLength.
//
//        $self reserve int:aLength.
//        
//        theBuffer move int:anIndex int:theLength int:aLength.
//        
//        aLiteral save int:anIndex chararray:theBuffer vint:aLength.
//        
//        theLength append int:aLength.
//    ]
//
//    insert int:anIndex wide:aLiteral
//    [
//        int aLength := 0.
//        UTF16Encoding charCount wide:aLiteral vint:aLength.
//
//        $self reserve int:aLength.
//        
//        theBuffer move int:anIndex int:theLength int:aLength.
//        
//        aLiteral save int:anIndex chararray:theBuffer vint:aLength.
//        
//        theLength append int:aLength.
//    ]
//    
//    insert int:anIndex char:aChar
//    [
//        $self reserve int:1.
//        
//        theBuffer move int:anIndex int:theLength int:1.
//        
//        theBuffer write int:anIndex char:aChar.
//        
//        theLength append int:1.
//    ]
//
//    insert : anObject at:anIndex
//        <= insert(anIndex int, anObject).
//
//    stacksafe indexOf int:aPosition char:aValue vint:aRetVal
//    [
//        int  anIndex := 0.
//        char aCurrent := $0.
//        
//        while (anIndex < theLength)
//        [
//            theBuffer read int:anIndex vchar:aCurrent.
//            
//            if (aCurrent == aValue)
//                [ aRetVal int := anIndex. ^$self ].
//                
//            anIndex += 1
//        ].
//        
//        aRetVal int := -1
//    ]
//
//    stacksafe indexOf int:aPosition literal:aLiteral vint:aRetVal
//    [
//        int  aLiteralLength := 0.
//        int  anIndex := 0.
//        int  aFirstCharLen := 0.
//        char aCurrent := $0.
//        char aFirstChar := $0.
//        char aNextChar := $0.
//        
//        aLiteral readLengthTo vint:aLiteralLength.
//        
//        aLiteral read int:0 vchar:aFirstChar.
//        aFirstChar readLengthTo vint:aFirstCharLen.
//        
//        int aMaxIndex := theLength.
//        aMaxIndex := aMaxIndex - aLiteralLength.
//        
//        while (anIndex < aMaxIndex)
//        [
//            theBuffer read int:anIndex vchar:aCurrent.
//            
//            if (aCurrent == aFirstChar)
//            [ 
//                int j := aFirstCharLen.
//                int chLen := 0.
//                while (j < aLiteralLength)
//                [
//                    theBuffer read int:(anIndex + j) vchar:aCurrent.
//                    aLiteral read int:j vchar:aNextChar.
//                    
//                    if (aNextChar == aCurrent)
//                        [ aNextChar readLengthTo vint:chLen ];
//                        [ j := aLiteralLength. chLen := 1 ].
//                    
//                    j := j + chLen
//                ].
//                
//                if (j == aLiteralLength)
//                    [ aRetVal int := anIndex. ^ $self ]
//            ].
//                
//            anIndex := anIndex + 1
//        ].
//        
//        aRetVal int := -1
//    ]
//
//    stacksafe indexOf int:aPosition wide:aLiteral vint:aRetVal
//    [
//        int  aLiteralLength := 0.
//        int  anIndex := 0.
//        int  aFirstCharLen := 0.
//        char aCurrent := $0.
//        char aFirstChar := $0.
//        char aNextChar := $0.
//        
//        aLiteral readLengthTo vint:aLiteralLength.
//        
//        aLiteral read int:0 vchar:aFirstChar.
//        aFirstChar readWideLengthTo vint:aFirstCharLen.
//        
//        while (anIndex < theLength)
//        [
//            theBuffer read int:anIndex vchar:aCurrent.
//            
//            if (aCurrent == aFirstChar)
//            [ 
//                int j := aFirstCharLen.
//                int chLen := 0.
//                while (j < aLiteralLength)
//                [
//                    theBuffer read int:(anIndex + j) vchar:aCurrent.
//                    aLiteral read int:j vchar:aNextChar.
//                    
//                    if (aNextChar == aCurrent)
//                        [ aNextChar readWideLengthTo vint:chLen ];
//                        [ j := aLiteralLength. chLen := 1 ].
//                    
//                    j := j + chLen
//                ].
//                
//                if (j > aLiteralLength)
//                    [ aRetVal int := anIndex. ^ $self ]
//            ].
//                
//            anIndex := anIndex + 1
//        ].
//        
//        aRetVal int := -1
//    ]
//
//    indexOf int:aPosition char:aValue    
//    [
//        int aRetVal := 0.
//        $self indexOf int:aPosition char:aValue vint:aRetVal.
//        
//        ^ IntNumber new int:aRetVal
//    ]
//    
//    indexOf int:aPosition literal:aValue
//    [
//        int aRetVal := 0.
//        $self indexOf int:aPosition literal:aValue vint:aRetVal.
//        
//        ^ IntNumber new int:aRetVal
//    ]
//    
//    indexOf int:aPosition wide:aValue
//    [
//        int aRetVal := 0.
//        $self indexOf int:aPosition wide:aValue vint:aRetVal.
//        
//        ^ IntNumber new int:aRetVal
//    ]
//
//    stacksafe literal Substring int:anIndex int:aLength
//    [
//        if (aLength == 0)
//            [ ^ emptyLiteralValue ].
//            
//        ^ LiteralValue new int:anIndex int:aLength chararray:theBuffer
//    ]
//
//    stacksafe wide WideSubstring int:anIndex int:aLength
//    [
//        if (aLength == 0)
//            [ ^ emptyWideLiteralValue ].
//            
//        ^ WideLiteralValue new int:anIndex int:aLength chararray:theBuffer
//    ]
//
//    stacksafe delete int:anIndex int:aLength
//    [
//        int n := 0 - aLength.
//        int l := theLength.
//        l := l - anIndex.
//        l := l - aLength.
//        
//        theBuffer move int:(anIndex + aLength) int:l int:n.
//        
//        theLength reduce int:aLength
//    ]
//    
//    stacksafe trim int:n
//    [
//        theLength reduce int:n
//    ]
}
