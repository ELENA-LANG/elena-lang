import system'text.

// --- literalCultureOp ---

public extension<literal> literalCultureOp
{
    literal toUpper(Locale aLocale)
    [
        literal aRetVal := nil.
        
        int     i := 0.
        int     len := self length.
        
        while (len > 0)
        [
            int s_len := 32.
            if(s_len > len)
                [ s_len := len ].
            
            V<short,32> src_tmp.
            int         src_len := 32.
            
            V<short,32> dst_tmp.
            int         dst_len := 64.

            UTF16Encoding convert(i, &s_len, self, T<ShortArray>(src_tmp), &src_len).
                                    
            aLocale strToUpper(T<ShortArray>(src_tmp), src_len, T<ShortArray>(dst_tmp), &dst_len).

            if(nil == aRetVal)
                [ aRetVal := UTF16Encoding toLiteral(0, dst_len, T<ShortArray>(dst_tmp)) ];
                [ aRetVal := aRetVal add(UTF16Encoding toLiteral(0, dst_len, T<ShortArray>(dst_tmp))) ].
                
            i := i + s_len.
            len := len - s_len
        ].
        
        ^ aRetVal.
    ]
    
    literal toLower(Locale aLocale)
    [
        literal aRetVal := nil.
        
        int i := 0.
        int len := self length.
        
        while (len > 0)
        [
            int s_len := 32.
            (s_len > len)
                ? [ s_len := len. ].
            
            V<short,32> src_tmp.
            int         src_len := 32.
            
            V<short,32> dst_tmp.
            int         dst_len := 64.

            UTF16Encoding convert(i, &s_len, self, T<ShortArray>(src_tmp), &src_len).
                                    
            aLocale strToLower(T<ShortArray>(src_tmp), src_len, T<ShortArray>(dst_tmp), &dst_len).

            if(nil == aRetVal)
                [ aRetVal := UTF16Encoding toLiteral(0, dst_len, T<ShortArray>(dst_tmp)) ];
                [ aRetVal := aRetVal add(UTF16Encoding toLiteral(0, dst_len, T<ShortArray>(dst_tmp))) ].
                
            i := i + s_len.
            len := len - s_len
        ].
        
        ^ aRetVal.
    ]
}

// --- wideCultureOp ---

public extension<wide> wideCultureOp
{
    wide toUpper(Locale aLocale)
    [
        wide aRetVal := nil.
        
        int i := 0.
        int len := self length.
        
        while (len > 0)
        [
            V<short,32> src_tmp.
            int         src_len := 32.
            
            if(src_len > len)
                [ src_len := len ].
            
            V<short,32> dst_tmp.
            int         dst_len := 64.
            
            self saveToShortArray(i, T<ShortArray>(src_tmp), src_len).
            
            aLocale strToUpper(T<ShortArray>(src_tmp), src_len, T<ShortArray>(dst_tmp), &dst_len).
            
            if(nil == aRetVal)
                [ aRetVal := WideLiteralValue fromShortArray(0, dst_len, T<ShortArray>(dst_tmp)) ];
                [ aRetVal := aRetVal add(WideLiteralValue fromShortArray(0, dst_len, T<ShortArray>(dst_tmp))) ].
                
            i := i + src_len.
            len := len - src_len
        ].
        
        ^ aRetVal
    ]
    
    wide toLower(Locale aLocale)
    [
        wide aRetVal := nil.
        
        int i := 0.
        int len := self length.
        
        while (len > 0)
        [
            V<short,32> src_tmp.
            int         src_len := 32.
            
            if(src_len > len)
                [ src_len := len ].
            
            V<short,32> dst_tmp.
            int         dst_len := 64.
            
            self saveToShortArray(i, T<ShortArray>(src_tmp), src_len).
            
            aLocale strToLower(T<ShortArray>(src_tmp), src_len, T<ShortArray>(dst_tmp), &dst_len).
            
            if(nil == aRetVal)
                [ aRetVal := WideLiteralValue fromShortArray(0, dst_len, T<ShortArray>(dst_tmp)) ];
                [ aRetVal := aRetVal add(WideLiteralValue fromShortArray(0, dst_len, T<ShortArray>(dst_tmp))) ].
                
            i := i + src_len.
            len := len - src_len
        ].
        
        ^ aRetVal.
    ]
}

// --- stringCultureOp ---

class stringCultureDispatcher =
{
    upperCase(literal l, Locale aLocale)
        = l~literalCultureOp toUpper(aLocale).

    lowerCase(literal l, Locale aLocale)
        = l~literalCultureOp toLower(aLocale).

    upperCase(wide l, Locale aLocale)
        = l~wideCultureOp toUpper(aLocale).

    lowerCase(wide l, Locale aLocale)
        = l~wideCultureOp toUpper(aLocale).
}.

public extension stringCultureOp
{
    toUpper(Locale aLocale)
        = stringCultureDispatcher upperCase(self, aLocale).
    
    toLower(Locale aLocale)
        = stringCultureDispatcher lowerCase(self, aLocale).
}