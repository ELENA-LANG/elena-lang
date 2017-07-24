#import system.
#import system'text.

// --- literalCultureOp ---

extension(literal) literalCultureOp
{
    literal toUpper &locale:aLocale
    [
        literal aRetVal := $nil.
        
        int     i := 0.
        int     len := self length.
        
        while (len > 0)
        [
            int s_len := 32.
            (s_len > len)
                ? [ s_len := len. ].
            
            short(32) src_tmp := 0.
            int       src_len := 32.
            
            short(32) dst_tmp := 0.
            int       dst_len := 64.

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
    
    literal toLower &locale:aLocale
    [
        literal aRetVal := $nil.
        
        int i := 0.
        int len := self length.
        
        while (len > 0)
        [
            int s_len := 32.
            (s_len > len)
                ? [ s_len := len. ].
            
            short(32) src_tmp := 0.
            int       src_len := 32.
            
            short(32) dst_tmp := 0.
            int       dst_len := 64.

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

extension(wide) wideCultureOp
{
    wide toUpper &locale:aLocale
    [
        wide aRetVal := $nil.
        
        int i := 0.
        int len := self length.
        
        while (len > 0)
        [
            short(32) src_tmp := 0.
            int       src_len := 32.
            (src_len > len)
                ? [ src_len := len. ].
            
            short(32) dst_tmp := 0.
            int       dst_len := 64.
            
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
    
    wide toLower &locale:aLocale
    [
        wide aRetVal := $nil.
        
        int i := 0.
        int len := self length.
        
        while (len > 0)
        [
            short(32) src_tmp := 0.
            int       src_len := 32.
            (src_len > len)
                ? [ src_len := len. ].
            
            short(32) dst_tmp := 0.
            int       dst_len := 64.
            
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

class stringCultureDispatcher =
{
    eval &literal:aLiteral = literalCultureOp.
    
    eval &wide:aLiteral = wideCultureOp.
}.

extension stringCultureOp
{
    toUpper &locale:aLocale
    [
        var dispatcher := self cast:%eval &to:stringCultureDispatcher.
        
        ^ self::dispatcher toUpper &locale:aLocale.
    ]
    
    toLower &locale:aLocale
    [
        var dispatcher := self cast:%eval &to:stringCultureDispatcher.
        
        ^ self::dispatcher toLower &locale:aLocale.
    ]
}