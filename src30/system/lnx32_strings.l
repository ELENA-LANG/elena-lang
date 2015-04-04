// --- characterController ---

#symbol characterController =
{
   idle [] // !! temporal

/*
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
    
    toUpperCase &char:aChar &vchar:aRetVal
    [
        #var(type:vint)aLength.
        #var(type:shortarray,size:3)aBuffer.

        charConvertor save &char:aChar &index:0 &shortarray:aBuffer &vlength:aLength.
        
        aBuffer write &index:aLength &short:0.
        
        system'external'USER32 CharUpperW &shortarray:aBuffer.
        
        charConvertor $convert &index:0 &shortarray:aBuffer &vchar:aRetVal.
    ]
    
    toLowerCase &char:aChar &vchar:aRetVal
    [
        #var(type:vint)aLength.
        #var(type:shortarray,size:3)aBuffer.

        charConvertor save &char:aChar &index:0 &shortarray:aBuffer &vlength:aLength.
        
        aBuffer write &index:aLength &short:0.
        
        system'external'USER32 CharLowerW &shortarray:aBuffer.
        
        charConvertor $convert &index:0 &shortarray:aBuffer &vchar:aRetVal.
    ]
*/
}.