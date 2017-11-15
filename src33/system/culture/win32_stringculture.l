import system'text.

// --- literalCultureOp ---

literal extension literalCultureOp
{
    literal toUpper locale:aLocale
    [
        literal aRetVal := $nil.
        
        int     i := 0.
        int     len := self length.
        
        while (len > 0)
        [
            int s_len := 32.
            if(s_len > len)
                [ s_len := len ].
            
            short& src_tmp(32).
            int    src_len := 32.
            
            short& dst_tmp(32).
            int    dst_len := 64.

            UTF16Encoding convert int:i vint:s_len literal:self shortarray:src_tmp vint:src_len.
                                    
            aLocale strToUpper shortarray:src_tmp int:src_len shortarray:dst_tmp vint:dst_len.

            if($nil == aRetVal)
                [ aRetVal := UTF16Encoding toLiteral int:0 int:dst_len shortarray:dst_tmp ];
                [ aRetVal := aRetVal add literal(UTF16Encoding toLiteral int:0 int:dst_len shortarray:dst_tmp) ].
                
            i := i + s_len.
            len := len - s_len
        ].
        
        ^ aRetVal.
    ]
    
    literal toLower locale:aLocale
    [
        literal aRetVal := $nil.
        
        int i := 0.
        int len := self length.
        
        while (len > 0)
        [
            int s_len := 32.
            (s_len > len)
                ? [ s_len := len. ].
            
            short& src_tmp(32).
            int    src_len := 32.
            
            short& dst_tmp(32).
            int    dst_len := 64.

            UTF16Encoding convert int:i vint:s_len literal:self shortarray:src_tmp vint:src_len.
                                    
            aLocale strToLower shortarray:src_tmp int:src_len shortarray:dst_tmp vint:dst_len.

            if($nil == aRetVal)
                [ aRetVal := UTF16Encoding toLiteral int:0 int:dst_len shortarray:dst_tmp ];
                [ aRetVal := aRetVal add literal(UTF16Encoding toLiteral int:0 int:dst_len shortarray:dst_tmp) ].
                
            i := i + s_len.
            len := len - s_len
        ].
        
        ^ aRetVal.
    ]
}

// --- wideCultureOp ---

wide extension wideCultureOp
{
    wide toUpper locale:aLocale
    [
        wide aRetVal := $nil.
        
        int i := 0.
        int len := self length.
        
        while (len > 0)
        [
            short& src_tmp(32).
            int    src_len := 32.
            if(src_len > len)
                [ src_len := len ].
            
            short& dst_tmp(32).
            int    dst_len := 64.
            
            self $save int:i shortarray:src_tmp int:src_len.
            
            aLocale strToUpper shortarray:src_tmp int:src_len shortarray:dst_tmp vint:dst_len.
            
            if($nil == aRetVal)
                [ aRetVal := WideLiteralValue $new int:0 int:dst_len shortarray:dst_tmp ];
                [ aRetVal := aRetVal add wide(WideLiteralValue $new int:0 int:dst_len shortarray:dst_tmp) ].
                
            i := i + src_len.
            len := len - src_len
        ].
        
        ^ aRetVal
    ]
    
    wide toLower locale:aLocale
    [
        wide aRetVal := $nil.
        
        int i := 0.
        int len := self length.
        
        while (len > 0)
        [
            short& src_tmp(32).
            int    src_len := 32.
            if(src_len > len)
                [ src_len := len ].
            
            short& dst_tmp(32).
            int    dst_len := 64.
            
            self $save int:i shortarray:src_tmp int:src_len.
            
            aLocale strToLower shortarray:src_tmp int:src_len shortarray:dst_tmp vint:dst_len.
            
            if($nil == aRetVal)
                [ aRetVal := WideLiteralValue $new int:0 int:dst_len shortarray:dst_tmp ];
                [ aRetVal := aRetVal add wide(WideLiteralValue $new int:0 int:dst_len shortarray:dst_tmp) ].
                
            i := i + src_len.
            len := len - src_len
        ].
        
        ^ aRetVal.
    ]
}

// --- stringCultureOp ---

class $stringCultureDispatcher =
{
    upperCase literal:l locale:aLocale
        = l~literalCultureOp toUpper locale:aLocale.

    lowerCase literal:l locale:aLocale
        = l~literalCultureOp toLower locale:aLocale.

    upperCase wide:l locale:aLocale
        = l~wideCultureOp toUpper locale:aLocale.

    lowerCase wide:l locale:aLocale
        = l~wideCultureOp toUpper locale:aLocale.
}.

extension stringCultureOp
{
    toUpper locale:aLocale
        = $stringCultureDispatcher upperCase(self, aLocale).
    
    toLower locale:aLocale
        = $stringCultureDispatcher lowerCase(self, aLocale).
}