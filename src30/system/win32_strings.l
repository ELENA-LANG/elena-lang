// --- characterController ---

#symbol $characterController =
{
    // !! should be corrected
    isUpperCase &char:aValue
    [
        #var(int)aRetVal := system'external'USER32 IsCharUpperW &char:aValue.
        
        ^ (aRetVal != 0).
    ]
    
    isLowerCase &char:aValue
    [
        #var(int)aRetVal := system'external'USER32 IsCharLowerW &char:aValue.
        
        ^ (aRetVal != 0).
    ]
    
    #method(stacksafe) toUpperCase &char:aChar &vchar:aRetVal
    [
        #var(int)aLength.
        #var(short:3)aBuffer.

        charConvertor $save &char:aChar &int:0 &shortarray:aBuffer &vint:aLength.

        aBuffer@aLength := 0.
        
        system'external'USER32 CharUpperW &shortarray:aBuffer.
        
        charConvertor $convert &int:0 &shortarray:aBuffer &vchar:aRetVal.
    ]
    
    #method(stacksafe) toLowerCase &char:aChar &vchar:aRetVal
    [
        #var(int)aLength.
        #var(short:3)aBuffer.

        charConvertor $save &char:aChar &int:0 &shortarray:aBuffer &vint:aLength.
        
        aBuffer@aLength := 0.
        
        system'external'USER32 CharLowerW &shortarray:aBuffer.
        
        charConvertor $convert &int:0 &shortarray:aBuffer &vchar:aRetVal.
    ]
}.

// --- caseLitrealOp ---

#class(extension:literal)caseLiteralOp
{
    #method upperCase
    [
        #var(literal)aRetVal := emptyLiteralValue.        
        #var(int)anIndex := 0.
        #var(char)aCurrent.
        #var(char)aConverted.
        #var(int)aCurrentLen.
        
        #var(int)aLength := self length.
        
        #loop (anIndex < aLength)?
        [
            self read &int:anIndex &vchar:aCurrent.
            aCurrentLen := aCurrent length.
            
            $characterController toUpperCase &char:aCurrent &vchar:aConverted.
            
            aRetVal := aRetVal add &char:aConverted.
            
            anIndex := anIndex + aCurrentLen.
        ].
            
        ^ aRetVal.
    ]

    #method lowerCase
    [
        #var(literal)aRetVal := emptyLiteralValue.        
        #var(int)anIndex := 0.
        #var(char)aCurrent.
        #var(char)aConverted.
        #var(int)aCurrentLen.
        
        #var(int)aLength := self length.
        
        #loop (anIndex < aLength)?
        [
            self read &int:anIndex &vchar:aCurrent.
            aCurrentLen := aCurrent length.
            
            $characterController toLowerCase &char:aCurrent &vchar:aConverted.
            
            aRetVal := aRetVal add &char:aConverted.
            
            anIndex := anIndex + aCurrentLen.
        ].
            
        ^ aRetVal.
    ]    
}

// --- caseLitrealOp ---

#class(extension:wide)caseWideOp
{
    #method upperCase
    [
        #var(wide)aRetVal := emptyWideLiteralValue.        
        #var(int)anIndex := 0.
        #var(char)aCurrent.
        #var(char)aConverted.
        #var(int)aCurrentLen.

        #var(int)aLength := self length.
                
        #loop (anIndex < aLength)?
        [
            self read &int:anIndex &vchar:aCurrent.
            aCurrentLen := aCurrent length.
            
            $characterController toUpperCase &char:aCurrent &vchar:aConverted.
            
            aRetVal := aRetVal add &char:aConverted.
            
            anIndex := anIndex + aCurrentLen.
        ].
            
        ^ aRetVal.
    ]

    #method lowerCase
    [
        #var(wide)aRetVal := emptyWideLiteralValue.        
        #var(int)anIndex := 0.
        #var(char)aCurrent.
        #var(char)aConverted.
        #var(int)aCurrentLen.
        
        #var(int)aLength := self length.
        
        #loop (anIndex < aLength)?
        [
            self read &int:anIndex &vchar:aCurrent.
            aCurrentLen := aCurrent length.
            
            $characterController toLowerCase &char:aCurrent &vchar:aConverted.
            
            aRetVal := aRetVal add &char:aConverted.
            
            anIndex := anIndex + aCurrentLen.
        ].
            
        ^ aRetVal.
    ]
}