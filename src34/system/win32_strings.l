public literal const newLine
    = $13$10.

// --- characterController ---

singleton characterController
{
    // !! should be corrected
    isUpperCase(char aValue)
    [
        int aRetVal := system'external'USER32 IsCharUpperW(aValue).
        
        ^ (aRetVal != 0)
    ]
    
    isLowerCase(char aValue)
    [
        int aRetVal := system'external'USER32 IsCharLowerW(aValue).
        
        ^ (aRetVal != 0)
    ]
    
    toUpperCase(char aChar, ref<char> aRetVal)
    [
        int        aLength := 0.
        T<short,3> aBuffer.

        charConvertor save(aChar, 0, T<ShortArray>(aBuffer), &aLength).

        aBuffer[aLength] := 0.
        
        system'external'USER32 CharUpperW(T<ShortArray>(aBuffer)).
        
        charConvertor convert(0, T<ShortArray>(aBuffer), aRetVal)
    ]
    
    toLowerCase(char aChar, ref<char> aRetVal)
    [
        int        aLength := 0.
        T<short,3> aBuffer.

        charConvertor save(aChar, 0, T<ShortArray>(aBuffer), &aLength).
        
        aBuffer[aLength] := 0.
        
        system'external'USER32 CharLowerW(T<ShortArray>(aBuffer)).
        
        charConvertor convert(0, T<ShortArray>(aBuffer), aRetVal)
    ]
}

// --- caseLitrealOp ---

public extension<literal> caseLiteralOp
{
    upperCase
    [
        literal aRetVal := emptyLiteral.
        int     anIndex := 0.
        char    aCurrent := $0.
        char    aConverted := $0.
        int     aCurrentLen := 0.
        
        int     aLength := self length.
        
        while (anIndex < aLength)
        [
            self read(anIndex, &aCurrent).
            aCurrentLen := aCurrent length.
            
            characterController toUpperCase(aCurrent, &aConverted).
            
            aRetVal := aRetVal add(aConverted).
            
            anIndex := anIndex + aCurrentLen
        ].
            
        ^ aRetVal
    ]

    lowerCase
    [
        literal aRetVal := emptyLiteral.        
        int     anIndex := 0.
        char    aCurrent := $0.
        char    aConverted := $0.
        int     aCurrentLen := 0.
        
        int     aLength := self length.
        
        while (anIndex < aLength)
        [
            self read(anIndex, &aCurrent).
            aCurrentLen := aCurrent length.
            
            characterController toLowerCase(aCurrent, &aConverted).
            
            aRetVal := aRetVal add(aConverted).
            
            anIndex := anIndex + aCurrentLen
        ].
            
        ^ aRetVal
    ]    
}

// --- caseLitrealOp ---

public extension<wide> caseWideOp
{
    upperCase
    [
        wide aRetVal := emptyWideLiteral.        
        int  anIndex := 0.
        char aCurrent := $0.
        char aConverted := $0.
        int  aCurrentLen := 0.

        int  aLength := self length.
                
        while (anIndex < aLength)
        [
            self read(anIndex, &aCurrent).
            aCurrentLen := aCurrent length.
            
            characterController toUpperCase(aCurrent, &aConverted).
            
            aRetVal := aRetVal add(aConverted).
            
            anIndex := anIndex + aCurrentLen
        ].
            
        ^ aRetVal
    ]

    lowerCase
    [
        wide aRetVal := emptyWideLiteral.        
        int  anIndex := 0.
        char aCurrent := $0.
        char aConverted := $0.
        int  aCurrentLen := 0.
        
        int  aLength := self length.
        
        while (anIndex < aLength)
        [
            self read(anIndex, &aCurrent).
            aCurrentLen := aCurrent length.
            
            characterController toLowerCase(aCurrent, &aConverted).
            
            aRetVal := aRetVal add(aConverted).
            
            anIndex := anIndex + aCurrentLen
        ].
            
        ^ aRetVal
    ]
}