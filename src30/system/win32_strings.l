// ==== ver 3.4.1 ===

// --- characterController ---

#symbol $characterController =
{
    // !! should be corrected
    isUpperCase &char:aValue
    [
        #var(type:int)aRetVal := system'external'USER32 IsCharUpperW &char:aValue.
        
        ^ (aRetVal != 0).
    ]
    
    isLowerCase &char:aValue
    [
        #var(type:int)aRetVal := system'external'USER32 IsCharLowerW &char:aValue.
        
        ^ (aRetVal != 0).
    ]
    
    #method(stacksafe) toUpperCase &char:aChar &vchar:aRetVal
    [
        #var(type:int)aLength.
        #var(type:shortarray,size:3)aBuffer.

        charConvertor $save &char:aChar &int:0 &shortarray:aBuffer &vint:aLength.
        
        aBuffer write &int:aLength &short:0.
        
        system'external'USER32 CharUpperW &shortarray:aBuffer.
        
        charConvertor $convert &int:0 &shortarray:aBuffer &vchar:aRetVal.
    ]
    
    #method(stacksafe) toLowerCase &char:aChar &vchar:aRetVal
    [
        #var(type:int)aLength.
        #var(type:shortarray,size:3)aBuffer.

        charConvertor $save &char:aChar &int:0 &shortarray:aBuffer &vint:aLength.
        
        aBuffer write &int:aLength &short:0.
        
        system'external'USER32 CharLowerW &shortarray:aBuffer.
        
        charConvertor $convert &int:0 &shortarray:aBuffer &vchar:aRetVal.
    ]
}.

// --- caseLitrealOp ---

#class(extension:literal)caseLiteralOp
{
    #method upperCase
    [
        #var(type:literal)aLiteral := self literal.
        
        #var(type:literal)aRetVal := emptyLiteralValue.        
        #var(type:int)anIndex := 0.
        #var(type:char)aCurrent.
        #var(type:char)aConverted.
        #var(type:int)aCurrentLen.
        
        #var(type:int)aLength := aLiteral length.
        
        #loop (anIndex < aLength)?
        [
            aLiteral read &int:anIndex &vchar:aCurrent.
            aCurrentLen := aCurrent length.
            
            $characterController toUpperCase &char:aCurrent &vchar:aConverted.
            
            aRetVal := aRetVal add &char:aConverted.
            
            anIndex := anIndex + aCurrentLen.
        ].
            
        ^ aRetVal.
    ]

    #method lowerCase
    [
        #var(type:literal)aLiteral := self literal.
        
        #var(type:literal)aRetVal := emptyLiteralValue.        
        #var(type:int)anIndex := 0.
        #var(type:char)aCurrent.
        #var(type:char)aConverted.
        #var(type:int)aCurrentLen.
        
        #var(type:int)aLength := aLiteral length.
        
        #loop (anIndex < aLength)?
        [
            aLiteral read &int:anIndex &vchar:aCurrent.
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
        #var(type:wide)aLiteral := self wide.
        
        #var(type:wide)aRetVal := emptyWideLiteralValue.        
        #var(type:int)anIndex := 0.
        #var(type:char)aCurrent.
        #var(type:char)aConverted.
        #var(type:int)aCurrentLen.

        #var(type:int)aLength := aLiteral length.
                
        #loop (anIndex < aLength)?
        [
            aLiteral read &int:anIndex &vchar:aCurrent.
            aCurrentLen := aCurrent length.
            
            $characterController toUpperCase &char:aCurrent &vchar:aConverted.
            
            aRetVal := aRetVal add &char:aConverted.
            
            anIndex := anIndex + aCurrentLen.
        ].
            
        ^ aRetVal.
    ]

    #method lowerCase
    [
        #var(type:wide)aLiteral := self wide.
        
        #var(type:wide)aRetVal := emptyWideLiteralValue.        
        #var(type:int)anIndex := 0.
        #var(type:char)aCurrent.
        #var(type:char)aConverted.
        #var(type:int)aCurrentLen.
        
        #var(type:int)aLength := aLiteral length.
        
        #loop (anIndex < aLength)?
        [
            aLiteral read &int:anIndex &vchar:aCurrent.
            aCurrentLen := aCurrent length.
            
            $characterController toLowerCase &char:aCurrent &vchar:aConverted.
            
            aRetVal := aRetVal add &char:aConverted.
            
            anIndex := anIndex + aCurrentLen.
        ].
            
        ^ aRetVal.
    ]
}