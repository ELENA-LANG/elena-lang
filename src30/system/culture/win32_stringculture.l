#import system.
#import system'text.

// --- literalCultureOp ---

#class(extension:literal)literalCultureOp
{
    #method(type:literal) toUpper &locale:aLocale
    [
        #var(type:literal)aLiteral := self literal.
        #var(type:literal)aRetVal := $nil.
        
        #var(type:int)i := 0.
        #var(type:int)len.
        aLiteral readLength &vint:len.
        
        #loop (len > 0)?
        [
            #var(type:int)s_len := 32.
            (s_len > len)
                ? [ s_len := len. ].
            
            #var(type:shortarray,size:32)src_tmp.
            #var(type:int)src_len := 32.
            
            #var(type:shortarray,size:32)dst_tmp.
            #var(type:int)dst_len := 64.

            UTF16Encoding convert &int:i &vint:s_len &literal:aLiteral &shortarray:src_tmp &vint:src_len.
                                    
            aLocale strToUpper &shortarray:src_tmp &int:src_len &shortarray:dst_tmp &vint:dst_len.

            ($nil == aRetVal)
                ? [ aRetVal := UTF16Encoding toLiteral &int:0 &int:dst_len &shortarray:dst_tmp. ]
                ! [ aRetVal := aRetVal add &literal:(UTF16Encoding toLiteral &int:0 &int:dst_len &shortarray:dst_tmp). ].
                
            i := i + s_len.
            len := len - s_len.          
        ].
        
        ^ aRetVal.
    ]
    
    #method(type:literal) toLower &locale:aLocale
    [
        #var(type:literal)aLiteral := self literal.
        #var(type:literal)aRetVal := $nil.
        
        #var(type:int)i := 0.
        #var(type:int)len.
        aLiteral readLength &vint:len.
        
        #loop (len > 0)?
        [
            #var(type:int)s_len := 32.
            (s_len > len)
                ? [ s_len := len. ].
            
            #var(type:shortarray,size:32)src_tmp.
            #var(type:int)src_len := 32.
            
            #var(type:shortarray,size:32)dst_tmp.
            #var(type:int)dst_len := 64.

            UTF16Encoding convert &int:i &vint:s_len &literal:aLiteral &shortarray:src_tmp &vint:src_len.
                                    
            aLocale strToLower &shortarray:src_tmp &int:src_len &shortarray:dst_tmp &vint:dst_len.

            ($nil == aRetVal)
                ? [ aRetVal := UTF16Encoding toLiteral &int:0 &int:dst_len &shortarray:dst_tmp. ]
                ! [ aRetVal := aRetVal add &literal:(UTF16Encoding toLiteral &int:0 &int:dst_len &shortarray:dst_tmp). ].
                
            i := i + s_len.
            len := len - s_len.          
        ].
        
        ^ aRetVal.
    ]
}

// --- wideCultureOp ---

#class(extension:wide)wideCultureOp
{
    #method(type:wide) toUpper &locale:aLocale
    [
        #var(type:wide)aLiteral := self wide.
        #var(type:wide)aRetVal := $nil.
        
        #var(type:int)i := 0.
        #var(type:int)len.
        aLiteral readLength &vint:len.
        
        #loop (len > 0)?
        [
            #var(type:shortarray,size:32)src_tmp.
            #var(type:int)src_len := 32.
            (src_len > len)
                ? [ src_len := len. ].
            
            #var(type:shortarray,size:32)dst_tmp.
            #var(type:int)dst_len := 64.
            
            aLiteral $save &int:i &shortarray:src_tmp &int:src_len.
            
            aLocale strToUpper &shortarray:src_tmp &int:src_len &shortarray:dst_tmp &vint:dst_len.
            
            ($nil == aRetVal)
                ? [ aRetVal := WideLiteralValue $new &int:0 &int:dst_len &shortarray:dst_tmp. ]
                ! [ aRetVal := aRetVal add &wide:(WideLiteralValue $new &int:0 &int:dst_len &shortarray:dst_tmp). ].
                
            i := i + src_len.
            len := len - src_len.          
        ].
        
        ^ aRetVal.
    ]
    
    #method(type:wide) toLower &locale:aLocale
    [
        #var(type:wide)aLiteral := self wide.
        #var(type:wide)aRetVal := $nil.
        
        #var(type:int)i := 0.
        #var(type:int)len.
        aLiteral readLength &vint:len.
        
        #loop (len > 0)?
        [
            #var(type:shortarray,size:32)src_tmp.
            #var(type:int)src_len := 32.
            (src_len > len)
                ? [ src_len := len. ].
            
            #var(type:shortarray,size:32)dst_tmp.
            #var(type:int)dst_len := 64.
            
            aLiteral $save &int:i &shortarray:src_tmp &int:src_len.
            
            aLocale strToLower &shortarray:src_tmp &int:src_len &shortarray:dst_tmp &vint:dst_len.
            
            ($nil == aRetVal)
                ? [ aRetVal := WideLiteralValue $new &int:0 &int:dst_len &shortarray:dst_tmp. ]
                ! [ aRetVal := aRetVal add &wide:(WideLiteralValue $new &int:0 &int:dst_len &shortarray:dst_tmp). ].
                
            i := i + src_len.
            len := len - src_len.          
        ].
        
        ^ aRetVal.
    ]
}

// --- stringCultureOp ---

#symbol stringCultureDispatcher =
{
    eval &literal:aLiteral = literalCultureOp.
    
    eval &wide:aLiteral = wideCultureOp.
}.

#class(extension)stringCultureOp
{
    #method toUpper &locale:aLocale
    [
        #var dispatcher := self cast:%eval &to:stringCultureDispatcher.
        
        ^ self::dispatcher toUpper &locale:aLocale.
    ]
    
    #method toLower &locale:aLocale
    [
        #var dispatcher := self cast:%eval &to:stringCultureDispatcher.
        
        ^ self::dispatcher toLower &locale:aLocale.
    ]
}