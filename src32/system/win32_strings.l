// --- characterController ---

class $characterController =
{
    // !! should be corrected
    isUpperCase char:aValue
    [
        int aRetVal := system'external'USER32 IsCharUpperW char:aValue.
        
        ^ (aRetVal != 0)
    ]
    
    isLowerCase char:aValue
    [
        int aRetVal := system'external'USER32 IsCharLowerW char:aValue.
        
        ^ (aRetVal != 0)
    ]
    
    stacksafe toUpperCase char:aChar vchar:aRetVal
    [
        int   aLength := 0.
        short aBuffer(3).

        charConvertor $save char:aChar int:0 shortarray:aBuffer vint:aLength.

        aBuffer@aLength := 0.
        
        system'external'USER32 CharUpperW shortarray:aBuffer.
        
        charConvertor $convert int:0 shortarray:aBuffer vchar:aRetVal
    ]
    
    stacksafe toLowerCase char:aChar vchar:aRetVal
    [
        int   aLength := 0.
        short aBuffer(3).

        charConvertor $save char:aChar int:0 shortarray:aBuffer vint:aLength.
        
        aBuffer@aLength := 0.
        
        system'external'USER32 CharLowerW shortarray:aBuffer.
        
        charConvertor $convert int:0 shortarray:aBuffer vchar:aRetVal
    ]
}.

// --- caseLitrealOp ---

literal extension caseLiteralOp
{
    upperCase
    [
        literal aRetVal := emptyLiteralValue.        
        int     anIndex := 0.
        char    aCurrent := $0.
        char    aConverted := $0.
        int     aCurrentLen := 0.
        
        int     aLength := self length.
        
        while (anIndex < aLength)
        [
            self read int:anIndex vchar:aCurrent.
            aCurrentLen := aCurrent length.
            
            $characterController toUpperCase char:aCurrent vchar:aConverted.
            
            aRetVal := aRetVal add char:aConverted.
            
            anIndex := anIndex + aCurrentLen
        ].
            
        ^ aRetVal
    ]

    lowerCase
    [
        literal aRetVal := emptyLiteralValue.        
        int     anIndex := 0.
        char    aCurrent := $0.
        char    aConverted := $0.
        int     aCurrentLen := 0.
        
        int     aLength := self length.
        
        while (anIndex < aLength)
        [
            self read int:anIndex vchar:aCurrent.
            aCurrentLen := aCurrent length.
            
            $characterController toLowerCase char:aCurrent vchar:aConverted.
            
            aRetVal := aRetVal add char:aConverted.
            
            anIndex := anIndex + aCurrentLen
        ].
            
        ^ aRetVal
    ]    
}

// --- caseLitrealOp ---

wide extension caseWideOp
{
    upperCase
    [
        wide aRetVal := emptyWideLiteralValue.        
        int  anIndex := 0.
        char aCurrent := $0.
        char aConverted := $0.
        int  aCurrentLen := 0.

        int  aLength := self length.
                
        while (anIndex < aLength)
        [
            self read int:anIndex vchar:aCurrent.
            aCurrentLen := aCurrent length.
            
            $characterController toUpperCase char:aCurrent vchar:aConverted.
            
            aRetVal := aRetVal add char:aConverted.
            
            anIndex := anIndex + aCurrentLen
        ].
            
        ^ aRetVal
    ]

    lowerCase
    [
        wide aRetVal := emptyWideLiteralValue.        
        int  anIndex := 0.
        char aCurrent := $0.
        char aConverted := $0.
        int  aCurrentLen := 0.
        
        int  aLength := self length.
        
        while (anIndex < aLength)
        [
            self read int:anIndex vchar:aCurrent.
            aCurrentLen := aCurrent length.
            
            $characterController toLowerCase char:aCurrent vchar:aConverted.
            
            aRetVal := aRetVal add char:aConverted.
            
            anIndex := anIndex + aCurrentLen
        ].
            
        ^ aRetVal
    ]
}