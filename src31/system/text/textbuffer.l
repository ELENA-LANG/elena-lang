const int $TextPageSize = 128.

type textbuffer :: system'text'TextBuilder.

// --- TextBuffer ---

sealed nonstructural class TextBuilder
{
    chararray theBuffer.
    vint      theLength.

    constructor new
    [
        theBuffer := CharArray new int:$TextPageSize.
        theLength := 0.
    ]

    int length :: outnprop(readLengthTo).

    clear
    [
        theLength set int:0.
    ]

    stacksafe readLengthTo vint:aRetVal
    [
        aRetVal int := theLength.
    ]

    stacksafe setLength int:aLength
    [
        theLength int := aLength.
    ]

    set length:aLength
        <= setLength int:aLength.

    chararray = theBuffer.

    literal
        = LiteralValue new int:0 int:theLength chararray:theBuffer.

    wide
        = WideLiteralValue new int:0 int:theLength chararray:theBuffer.

    write : aLine
    [
        aLine cast:%write to:$self.
    ]

    writeLine
    [
        $self write:'newLine.
    ]

    append : aLine
    [
        $self write:aLine.
    ]
                                    
    stacksafe reserve int:aLength
    [
        int aNewLength := aLength.
        
        aNewLength := aNewLength + theLength.
        
        int aBufferLength := theBuffer length.
        
        if (aNewLength >= aBufferLength)
        [
            //aNewLength := (aNewLength / $TextPageSize + 1) * $TextPageSize.
            aNewLength := aNewLength / $TextPageSize.
            aNewLength := aNewLength + 1.
            aNewLength := aNewLength * $TextPageSize.            
            
            chararray aNewBuffer := CharArray new int:aNewLength.
            
            aNewBuffer write int:0 int:aBufferLength chararray:theBuffer.
            
            theBuffer := aNewBuffer.
        ].
    ]

    write literal:aLiteral
    [
        int aLength := aLiteral length.
        
        ifnot (0 == aLength)
        [
            $self reserve int:aLength.
            
            aLiteral save int:theLength chararray:theBuffer vint:aLength.
            
            theLength append int:aLength.
        ].
    ]

    write wide:aLiteral
    [
        int aLength := aLiteral length.
        
        ifnot (0 == aLength)
        [
            $self reserve int:aLength.
            
            aLiteral save int:theLength chararray:theBuffer vint:aLength.
            
            theLength append int:aLength.
        ].
    ]

    stacksafe write char:aChar
    [
        $self reserve int:1.
        
        theBuffer write int:theLength char:aChar.
        
        theLength append int:1.
    ]

    stacksafe write int:n
    [
        byte tmp(16).
        char ch_tmp(16).
        int  len := 16.
        
        literalConvertor convert int:n bytearray:tmp vint:len.
        
        UTF8Encoding convert int:0 vint:len bytearray:tmp chararray:ch_tmp vint:len.
        
        $self write chararray:ch_tmp int:len.
    ]

    generic write : anObject
    [
        $self write literal:anObject.
    ]

    stacksafe write chararray:anArray int:aLength
    [
        ifnot (0 == aLength)
        [
            $self reserve int:aLength.
            
            theBuffer write int:theLength int:aLength chararray:anArray.
            
            theLength append int:aLength.
        ].
    ]

    insert int:anIndex int:aLength chararray:anArray
    [
        $self reserve int:aLength.
        
        theBuffer move int:anIndex int:theLength int:aLength.
        
        theBuffer write int:anIndex int:aLength chararray:anArray.
        
        theLength append int:aLength.
    ]

    insert int:anIndex literal:aLiteral
    [
        int aLength := 0.
        UTF8Encoding charCount literal:aLiteral vint:aLength.

        $self reserve int:aLength.
        
        theBuffer move int:anIndex int:theLength int:aLength.
        
        aLiteral save int:anIndex chararray:theBuffer vint:aLength.
        
        theLength append int:aLength.
    ]

    insert int:anIndex wide:aLiteral
    [
        int aLength := 0.
        UTF16Encoding charCount wide:aLiteral vint:aLength.

        $self reserve int:aLength.
        
        theBuffer move int:anIndex int:theLength int:aLength.
        
        aLiteral save int:anIndex chararray:theBuffer vint:aLength.
        
        theLength append int:aLength.
    ]
    
    insert int:anIndex char:aChar
    [
        $self reserve int:1.
        
        theBuffer move int:anIndex int:theLength int:1.
        
        theBuffer write int:anIndex char:aChar.
        
        theLength append int:1.
    ]

    insert : anObject at:anIndex
        = anObject cast:%eval to:
            {
               eval:target literal:aLiteral
                  = target insert int:anIndex literal:aLiteral.
    
               eval:target wide:aLiteral
                  = target insert int:anIndex literal:aLiteral.
    
               eval:target char:aChar
                  = target insert int:anIndex char:aChar.
            } with:$self.

    stacksafe indexOf int:aPosition char:aValue vint:aRetVal
    [
        int  anIndex := 0.
        char aCurrent := $0.
        
        while (anIndex < theLength)
        [
            theBuffer read int:anIndex vchar:aCurrent.
            
            if (aCurrent == aValue)
                [ aRetVal int := anIndex. ^$self ].
                
            anIndex append int:1
        ].
        
        aRetVal int := -1
    ]

    stacksafe indexOf int:aPosition literal:aLiteral vint:aRetVal
    [
        int  aLiteralLength := 0.
        int  anIndex := 0.
        int  aFirstCharLen := 0.
        char aCurrent := $0.
        char aFirstChar := $0.
        char aNextChar := $0.
        
        aLiteral readLengthTo vint:aLiteralLength.
        
        aLiteral read int:0 vchar:aFirstChar.
        aFirstChar readLengthTo vint:aFirstCharLen.
        
        int aMaxIndex := theLength.
        aMaxIndex := aMaxIndex - aLiteralLength.
        
        while (anIndex < aMaxIndex)
        [
            theBuffer read int:anIndex vchar:aCurrent.
            
            if (aCurrent == aFirstChar)
            [ 
                int j := aFirstCharLen.
                int chLen := 0.
                while (j < aLiteralLength)
                [
                    theBuffer read int:(anIndex + j) vchar:aCurrent.
                    aLiteral read int:j vchar:aNextChar.
                    
                    if (aNextChar == aCurrent)
                        [ aNextChar readLengthTo vint:chLen ];
                        [ j := aLiteralLength. chLen := 1 ].
                    
                    j := j + chLen
                ].
                
                if (j == aLiteralLength)
                    [ aRetVal int := anIndex. ^ $self ]
            ].
                
            anIndex := anIndex + 1
        ].
        
        aRetVal int := -1
    ]

    stacksafe indexOf int:aPosition wide:aLiteral vint:aRetVal
    [
        int  aLiteralLength := 0.
        int  anIndex := 0.
        int  aFirstCharLen := 0.
        char aCurrent := $0.
        char aFirstChar := $0.
        char aNextChar := $0.
        
        aLiteral readLengthTo vint:aLiteralLength.
        
        aLiteral read int:0 vchar:aFirstChar.
        aFirstChar readWideLengthTo vint:aFirstCharLen.
        
        while (anIndex < theLength)
        [
            theBuffer read int:anIndex vchar:aCurrent.
            
            if (aCurrent == aFirstChar)
            [ 
                int j := aFirstCharLen.
                int chLen := 0.
                while (j < aLiteralLength)
                [
                    theBuffer read int:(anIndex + j) vchar:aCurrent.
                    aLiteral read int:j vchar:aNextChar.
                    
                    if (aNextChar == aCurrent)
                        [ aNextChar readWideLengthTo vint:chLen ];
                        [ j := aLiteralLength. chLen := 1 ].
                    
                    j := j + chLen
                ].
                
                if (j > aLiteralLength)
                    [ aRetVal int := anIndex. ^ $self ]
            ].
                
            anIndex := anIndex + 1
        ].
        
        aRetVal int := -1
    ]

    indexOf int:aPosition char:aValue    
    [
        int aRetVal := 0.
        $self indexOf int:aPosition char:aValue vint:aRetVal.
        
        ^ IntNumber new int:aRetVal
    ]
    
    indexOf int:aPosition literal:aValue
    [
        int aRetVal := 0.
        $self indexOf int:aPosition literal:aValue vint:aRetVal.
        
        ^ IntNumber new int:aRetVal
    ]
    
    indexOf int:aPosition wide:aValue
    [
        int aRetVal := 0.
        $self indexOf int:aPosition wide:aValue vint:aRetVal.
        
        ^ IntNumber new int:aRetVal
    ]

    stacksafe literal Substring int:anIndex int:aLength
    [
        if (aLength == 0)
            [ ^ emptyLiteralValue ].
            
        ^ LiteralValue new int:anIndex int:aLength chararray:theBuffer
    ]

    stacksafe wide WideSubstring int:anIndex int:aLength
    [
        if (aLength == 0)
            [ ^ emptyWideLiteralValue ].
            
        ^ WideLiteralValue new int:anIndex int:aLength chararray:theBuffer
    ]

    stacksafe delete int:anIndex int:aLength
    [
        int n := 0 - aLength.
        int l := theLength.
        l := l - anIndex.
        l := l - aLength.
        
        theBuffer move int:(anIndex + aLength) int:l int:n.
        
        theLength reduce int:aLength
    ]
}

symbol TextBuffer = TextBuilder.