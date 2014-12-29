// --- characterController ---

#symbol characterController =
{
    isUpperCase &short:aValue
    [
        #var(type:int)aCode := aValue int.
        #var(type:int)aRetVal := system'external'USER32 IsCharUpperW &int:aCode.
        
        ^ (aRetVal != 0).
    ]
    
    isLowerCase &short:aValue
    [
        #var(type:int)aCode := aValue int.
        #var(type:int)aRetVal := system'external'USER32 IsCharLowerW &int:aCode.
        
        ^ (aRetVal != 0).
    ]
    
    toUpperCase &shortarray:aBuffer &length:aLength
    [
        aBuffer write &index:aLength &short:0.
        
        system'external'USER32 CharUpperW &shortarray:aBuffer.
    ]
    
    toLowerCase &shortarray:aBuffer &length:aLength
    [
        aBuffer write &index:aLength &short:0.
        
        system'external'USER32 CharLowerW &shortarray:aBuffer.
    ]
}.