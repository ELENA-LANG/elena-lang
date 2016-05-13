#import system.
#import system'text.

// --- literalCultureOp ---

#class(extension:literal)literalCultureOp
{
    #method(literal) toUpper &locale:aLocale
    [
        #var(literal)aRetVal := $nil.
        
        #var(int)i := 0.
        #var(int)len := self length.
        
        #loop (len > 0)?
        [
            #var(int)s_len := 32.
            (s_len > len)
                ? [ s_len := len. ].
            
            #var(short:32)src_tmp.
            #var(int)src_len := 32.
            
            #var(short:32)dst_tmp.
            #var(int)dst_len := 64.

            UTF16Encoding convert &int:i &vint:s_len &literal:self &shortarray:src_tmp &vint:src_len.
                                    
            aLocale strToUpper &shortarray:src_tmp &int:src_len &shortarray:dst_tmp &vint:dst_len.

            ($nil == aRetVal)
                ? [ aRetVal := UTF16Encoding toLiteral &int:0 &int:dst_len &shortarray:dst_tmp. ]
                ! [ aRetVal := aRetVal add &literal:(UTF16Encoding toLiteral &int:0 &int:dst_len &shortarray:dst_tmp). ].
                
            i := i + s_len.
            len := len - s_len.          
        ].
        
        ^ aRetVal.
    ]
    
    #method(literal) toLower &locale:aLocale
    [
        #var(literal)aRetVal := $nil.
        
        #var(int)i := 0.
        #var(int)len := self length.
        
        #loop (len > 0)?
        [
            #var(int)s_len := 32.
            (s_len > len)
                ? [ s_len := len. ].
            
            #var(short:32)src_tmp.
            #var(int)src_len := 32.
            
            #var(short:32)dst_tmp.
            #var(int)dst_len := 64.

            UTF16Encoding convert &int:i &vint:s_len &literal:self &shortarray:src_tmp &vint:src_len.
                                    
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
    #method(wide) toUpper &locale:aLocale
    [
        #var(wide)aRetVal := $nil.
        
        #var(int)i := 0.
        #var(int)len := self length.
        
        #loop (len > 0)?
        [
            #var(short:32)src_tmp.
            #var(int)src_len := 32.
            (src_len > len)
                ? [ src_len := len. ].
            
            #var(short:32)dst_tmp.
            #var(int)dst_len := 64.
            
            self $save &int:i &shortarray:src_tmp &int:src_len.
            
            aLocale strToUpper &shortarray:src_tmp &int:src_len &shortarray:dst_tmp &vint:dst_len.
            
            ($nil == aRetVal)
                ? [ aRetVal := WideLiteralValue $new &int:0 &int:dst_len &shortarray:dst_tmp. ]
                ! [ aRetVal := aRetVal add &wide:(WideLiteralValue $new &int:0 &int:dst_len &shortarray:dst_tmp). ].
                
            i := i + src_len.
            len := len - src_len.          
        ].
        
        ^ aRetVal.
    ]
    
    #method(wide) toLower &locale:aLocale
    [
        #var(wide)aRetVal := $nil.
        
        #var(int)i := 0.
        #var(int)len := self length.
        
        #loop (len > 0)?
        [
            #var(short:32)src_tmp.
            #var(int)src_len := 32.
            (src_len > len)
                ? [ src_len := len. ].
            
            #var(short:32)dst_tmp.
            #var(int)dst_len := 64.
            
            self $save &int:i &shortarray:src_tmp &int:src_len.
            
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