// === basic interfaces ===

// --- Encoder ---

public interface Encoder
{
    stacksafe abstract convert(int anIndex, ref<int> aLength, ByteArray aByteArray, CharArray anOutput, ref<int> anOutputLength) [] 

    stacksafe abstract convert(int anIndex, ref<int> aLength, CharArray aCharArray, ByteArray anOutput, ref<int> anOutputLength) []
    
    stacksafe convert(int anIndex, ref<int> aLength, literal aLiteral, ByteArray anOutput, ref<int> anOutputLength)
    [
        V<char,64> temp.
        int        temp_len := 64.
        if (temp_len > aLength)
            [ temp_len := aLength value ];
            [ aLength value := temp_len ].
        
        aLiteral Substring(anIndex, T<int>(aLength)); save(0, T<CharArray>(temp), &temp_len).
        
        self convert(0, &temp_len, T<CharArray>(temp), anOutput, anOutputLength).
    ]
    
    stacksafe convert(int anIndex, ref<int> aLength, wide aLiteral, ByteArray anOutput, ref<int> anOutputLength)
    [
        V<char,64> temp.
        int        temp_len := 64.
        if (temp_len > aLength)
            [ temp_len := aLength value ];
            [ aLength value := temp_len ].
        
        aLiteral Substring(anIndex, aLength); save(0, T<CharArray>(temp), &temp_len).
        
        self convert(0, &temp_len, T<CharArray>(temp), anOutput, anOutputLength).
    ]
    
    stacksafe toByteArray(int anIndex, int aLength, literal aLiteral)
    [
        int          i := anIndex.
        T<ByteArray> aRetVal := nil.
        int          remaining := aLength.
        
        while (i < aLength)
        [
            V<byte,64> tmp.
            int        tmp_len := 64.
            int        len := remaining.
            if (len > tmp_len)
                [ len := tmp_len ].
             
            self convert(i, &len, aLiteral, T<ByteArray>(tmp), &tmp_len).
            
            if (nil == aRetVal)
                [ aRetVal := ByteArray new(0, tmp_len, T<ByteArray>(tmp)) ];
                [ aRetVal := aRetVal add(tmp_len, T<ByteArray>(tmp)) ].
            
            i := i + len.
            remaining := remaining - len
        ].
        
        ^ aRetVal
    ]
    
    stacksafe toByteArray(int anIndex, int aLength, wide aLiteral)
    [
        int          i := anIndex.
        T<ByteArray> aRetVal := nil.
        int          remaining := aLength.
        
        while (i < aLength)
        [
            V<byte,64> tmp.
            int        tmp_len := 64.
            int        len := remaining.
            if (len > tmp_len)
                [ len := tmp_len ].
             
            self convert(i, &len, aLiteral, T<ByteArray>(tmp), &tmp_len).
            
            if (nil == aRetVal)
                [ aRetVal := ByteArray new(0, tmp_len, T<ByteArray>(tmp)) ];
                [ aRetVal := aRetVal add(tmp_len, T<ByteArray>(tmp)) ].
            
            i := i + len.
            remaining := remaining - len
        ].
        
        ^ aRetVal
    ]
    
    stacksafe toLiteral(int anIndex, int aLength, ByteArray anArray)
    [
        int     i := anIndex.
        literal aRetVal := nil.
        int     remaining := aLength.
        
        while (i < aLength)
        [
            V<char,64> tmp.
            int        tmp_len := 64.
            int        len := remaining.
            if (len > tmp_len)
                [ len := tmp_len ].
             
            self convert(i, &len, anArray, T<CharArray>(tmp), &tmp_len).
            
            if (nil == aRetVal)
               [ aRetVal := LiteralValue new(0, tmp_len, T<CharArray>(tmp)) ];
               [ aRetVal := aRetVal add(LiteralValue new(0, tmp_len, T<CharArray>(tmp))) ].
            
            i := i + len.
            remaining := remaining - len.
        ].
        
        ^ aRetVal.
    ]
    
    stacksafe toWide(int anIndex, int aLength, ByteArray anArray)
    [
        int  i := anIndex.
        wide aRetVal := nil.
        int  remaining := aLength.
        
        while (i < aLength)
        [
            V<char,64> tmp.
            int        tmp_len := 64.
            int        len := remaining.
            if (len > tmp_len)
                [ len := tmp_len ].
             
            self convert(i, &len, anArray, T<CharArray>(tmp), &tmp_len).
            
            if (nil == aRetVal)
               [ aRetVal := WideLiteralValue new(0, tmp_len, T<CharArray>(tmp)) ];
               [ aRetVal := aRetVal add(WideLiteralValue new(0, tmp_len, T<CharArray>(tmp))) ].
            
            i := i + len.
            remaining := remaining - len
        ].
        
        ^ aRetVal
    ]
}

// --- UTF8Encoder ---

public singleton UTF8Encoder :: Encoder
{
    convert(int anIndex, ref<int> aLength, ByteArray aByteArray, CharArray anOutput, ref<int> anOutputLength)
        => system'core_routines'__utf8encoder.
    
    convert(int anIndex, ref<int> aLength, CharArray aByteArray, ByteArray anOutput, ref<int> anOutputLength)
        => system'core_routines'__utf8encoder.
    
    stacksafe convert(int anIndex, ref<int> aLength, literal aLiteral, ByteArray anOutput, ref<int> anOutputLength)
    [
        int l := aLength value.
        aLiteral saveToByteArray(anIndex, anOutput, l).

        anOutputLength value := aLength
    ]
    
    stacksafe toLiteral(int anIndex, int aLength, ByteArray anArray)
        = UTF8Encoding toLiteral(anIndex, aLength, anArray).
}

// --- UTF8Encoding ---

class UTF8Encoding =
{
    T<Encoder> encoder = UTF8Encoder.
    
    stacksafe charCount(literal aLiteral, ref<int> aRetVal)
        => system'core_routines'__utf8encoder.
        
    stacksafe convert(int anIndex, int aLength, literal aLiteral, ByteArray anArray)
    [
        aLiteral saveToByteArray(anIndex, anArray, aLength).
    ]
        
    stacksafe toLiteral(int anIndex, int aLength, ByteArray anArray)
        = LiteralValue fromByteArray(anIndex, aLength, anArray).
        
    stacksafe convert(int anIndex, ref<int> aLength, ByteArray aByteArray, CharArray anOutput, ref<int> anOutputLength)
        => system'core_routines'__utf8encoder.
        
    stacksafe convert(int anIndex, ref<int> aLength, literal aByteArray, CharArray anOutput, ref<int> anOutputLength)
        => system'core_routines'__utf8encoder.
}.

// --- UTF16Encoder ---

public singleton UTF16Encoder :: Encoder
{
    convert(int anIndex, ref<int> aLength, ByteArray aByteArray, CharArray anOutput, ref<int> anOutputLength)
        => system'core_routines'__utf16encoder.
    
    convert(int anIndex, ref<int> aLength, CharArray aByteArray, ByteArray anOutput, ref<int>anOutputLength)
        => system'core_routines'__utf16encoder.
}

// --- UTF16Encoding ---

public singleton UTF16Encoding
{
    T<Encoder> encoder = UTF16Encoder.
    
    stacksafe charCount(wide aLiteral, ref<int> aRetVal)
        => system'core_routines'__utf16encoder.
        
    stacksafe convert(int anIndex, int aLength, wide aLiteral, ShortArray anArray)
    [
        aLiteral saveToShortArray(anIndex, anArray, aLength).
    ]

    stacksafe convert(int anIndex, int aSize, ShortArray aByteArray, CharArray anOutput, ref<int> anOutputLength)
        => system'core_routines'__utf16encoder.
        
    stacksafe convert(int anIndex, int aSize, CharArray aByteArray, ShortArray anOutput, ref<int> anOutputLength)
        => system'core_routines'__utf16encoder.
        
    stacksafe convert(int anIndex, ref<int> aSize, literal aLiteral, ShortArray anOutput, ref<int> anOutputLength)
    [
        V<char,64> temp.
        int        temp_len := 64.
        if (temp_len < aSize)
            [ aSize value := temp_len ].
        
        UTF8Encoding convert(anIndex, aSize, aLiteral, T<CharArray>(temp), &temp_len).
        
        self convert(0, temp_len, T<CharArray>(temp), anOutput, anOutputLength)
    ]
        
    stacksafe literal toLiteral(int anIndex, int aSize, ShortArray anInput)
    [
        V<char,64> temp.
        int        temp_len := 64.
        
        self convert(anIndex, aSize, anInput, T<CharArray>(temp), &temp_len).
        
        ^ LiteralValue new(0, temp_len, T<CharArray>(temp))
    ]
}
