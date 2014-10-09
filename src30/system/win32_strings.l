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
    
    toUpperCase &literal:aLiteral
    [
        #var aRetVal := emptyLiteralValue.        
        #var(type:int)anIndex := 0.
        #var(type:shortarray,size:128)aTemp.
        #var(type:int)aLength.
        aLiteral read &out'length:aLength.
        
        (aLength < 128)
            ? [
                aTemp write &index:0 &length:(aLength + 1) &literal:aLiteral.
            
                system'external'USER32 CharUpperW &out'shortarray:aTemp.
                
                aRetVal := LiteralValue new &length:aLength &shortarray:aTemp.                
            ]
            ! [
                #loop (aLength > 0)?
                [
                    (aLength > 127)
                        ? [
                            aTemp write &index:anIndex &length:127 &literal:aLiteral.
                            aTemp write &index:128 &short:0.
                        
                            system'external'USER32 CharUpperW &out'shortarray:aTemp.
                            
                            aRetVal := aRetVal add &literal:(LiteralValue new &length:127 &shortarray:aTemp).                            
                            anIndex := anIndex + 127.
                            aLength := aLength - 127.
                        ]
                        ! [
                            aTemp write &index:anIndex &length:(aLength + 1) &literal:aLiteral.
                        
                            system'external'USER32 CharUpperW &out'shortarray:aTemp.
                            
                            aRetVal := aRetVal add &literal:(LiteralValue new &length:aLength &shortarray:aTemp).                            
                            aLength := 0.
                        ].
                ].
            ].
            
        ^ aRetVal.
    ]
    
    toLowerCase &literal:aLiteral
    [
        #var aRetVal := emptyLiteralValue.        
        #var(type:int)anIndex := 0.
        #var(type:shortarray,size:128)aTemp.
        #var(type:int)aLength.
        aLiteral read &out'length:aLength.
        
        (aLength < 128)
            ? [
                aTemp write &index:0 &length:(aLength + 1) &literal:aLiteral.
            
                system'external'USER32 CharLowerW &out'shortarray:aTemp.
                
                aRetVal := LiteralValue new &length:aLength &shortarray:aTemp.                
            ]
            ! [
                #loop (aLength > 0)?
                [
                    (aLength > 127)
                        ? [
                            aTemp write &index:anIndex &length:127 &literal:aLiteral.
                            aTemp write &index:128 &short:0.
                        
                            system'external'USER32 CharLowerW &out'shortarray:aTemp.
                            
                            aRetVal := aRetVal add &literal:(LiteralValue new &length:127 &shortarray:aTemp).                            
                            anIndex := anIndex + 127.
                            aLength := aLength - 127.
                        ]
                        ! [
                            aTemp write &index:anIndex &length:(aLength + 1) &literal:aLiteral.
                        
                            system'external'USER32 CharLowerW &out'shortarray:aTemp.
                            
                            aRetVal := aRetVal add &literal:(LiteralValue new &length:aLength &shortarray:aTemp).                            
                            aLength := 0.
                        ].
                ].
            ].
            
        ^ aRetVal.
    ]
}.