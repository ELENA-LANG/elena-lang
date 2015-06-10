// --- characterController ---

#symbol characterController =
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
    
    toUpperCase &char:aChar &vchar:aRetVal
    [
        #var(type:int)aLength.
        #var(type:shortarray,size:3)aBuffer.

        charConvertor $save &char:aChar &index:0 &shortarray:aBuffer &vlength:aLength.
        
        aBuffer write &index:aLength &short:0.
        
        system'external'USER32 CharUpperW &shortarray:aBuffer.
        
        charConvertor $convert &index:0 &shortarray:aBuffer &vchar:aRetVal.
    ]
    
    toLowerCase &char:aChar &vchar:aRetVal
    [
        #var(type:int)aLength.
        #var(type:shortarray,size:3)aBuffer.

        charConvertor $save &char:aChar &index:0 &shortarray:aBuffer &vlength:aLength.
        
        aBuffer write &index:aLength &short:0.
        
        system'external'USER32 CharLowerW &shortarray:aBuffer.
        
        charConvertor $convert &index:0 &shortarray:aBuffer &vchar:aRetVal.
    ]
}.