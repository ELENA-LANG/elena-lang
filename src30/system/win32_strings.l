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
    
    toUpperCase &char:aChar &out'char:aRetVal
    [
        #var s := aChar wide.
        
        #var(type:shortarray,size:3)aBuffer.
        aBuffer write &index:0 &wide:(aChar wide).
        
        system'external'USER32 CharUpperW &shortarray:aBuffer.
        
        aRetVal := WideLiteralValue new &length:2 &shortarray:aBuffer getAt &index:0.
    ]
    
    toLowerCase &char:aChar &out'char:aRetVal
    [
        #var s := aChar wide.
        
        #var(type:shortarray,size:3)aBuffer.
        aBuffer write &index:0 &wide:(aChar wide).
        
        system'external'USER32 CharLowerW &shortarray:aBuffer.
        
        aRetVal := WideLiteralValue new &length:2 &shortarray:aBuffer getAt &index:0.
    ]
}.