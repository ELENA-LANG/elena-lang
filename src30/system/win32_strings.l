// --- characterController ---

#symbol characterController =
{
    isUpperCase &char:aValue
    [
        #var(type:int)aCode := aValue int.
        #var(type:int)aRetVal := system'external'USER32 IsCharUpperW &int:aCode.
        
        ^ (aRetVal != 0).
    ]
    
    isLowerCase &char:aValue
    [
        #var(type:int)aCode := aValue int.
        #var(type:int)aRetVal := system'external'USER32 IsCharLowerW &int:aCode.
        
        ^ (aRetVal != 0).
    ]
    
    toUpperCase &chararray:aBuffer &length:aLength
    [
        aBuffer write &index:aLength &char:#0.
        
        system'external'USER32 CharUpperW &chararray:aBuffer.
    ]
    
    toLowerCase &chararray:aBuffer &length:aLength
    [
        aBuffer write &index:aLength &char:#0.
        
        system'external'USER32 CharLowerW &chararray:aBuffer.
    ]
}.