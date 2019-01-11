// === basic interfaces ===

// --- Encoder ---

public interface Encoder
{
    abstract convert(int index, ref int length, byte[] byteArray, char[] output, ref int outputLength) {} 

    abstract convert(int index, ref int length, char[] charArray, byte[] output, ref int outputLength) {}
    
    convert(int index, ref int length, string s, byte[] output, ref int outputLength)
    {
        char temp[64];
        int  temp_len := 64;
        if (temp_len > length)
        {
            temp_len := length
        }
        else
        {
            length := temp_len
        };
        
        s.Substring(index, length).save(0, temp, ref temp_len);
        
        self.convert(0, ref temp_len, temp, output, ref outputLength)
    }
    
//    convert(int ondex, ref int length, wide s, byte[] output, ref int outputLength)
//    [
//        char temp[64];
//        int  temp_len := 64;
//        if (temp_len > length)
//        {
//            temp_len := length
//        }
//        else
//        {
//            length := temp_len
//        };
//        
//        s.Substring(index, length).save(0, temp, ref temp_len);
//        
//        self.convert(0, ref temp_len, temp, output, ref outputLength).
//    ]
//    
//    toByteArray(int anIndex, int aLength, literal aLiteral)
//    [
//        int          i := anIndex.
//        T<ByteArray> aRetVal := nil.
//        int          remaining := aLength.
//        
//        while (i < aLength)
//        [
//            V<byte,64> tmp.
//            int        tmp_len := 64.
//            int        len := remaining.
//            if (len > tmp_len)
//                [ len := tmp_len ].
//             
//            self convert(i, &len, aLiteral, T<ByteArray>(tmp), &tmp_len).
//            
//            if (nil == aRetVal)
//                [ aRetVal := ByteArray new(0, tmp_len, T<ByteArray>(tmp)) ];
//                [ aRetVal := aRetVal add(tmp_len, T<ByteArray>(tmp)) ].
//            
//            i := i + len.
//            remaining := remaining - len
//        ].
//        
//        ^ aRetVal
//    ]
//    
//    toByteArray(int anIndex, int aLength, wide aLiteral)
//    [
//        int          i := anIndex.
//        T<ByteArray> aRetVal := nil.
//        int          remaining := aLength.
//        
//        while (i < aLength)
//        [
//            V<byte,64> tmp.
//            int        tmp_len := 64.
//            int        len := remaining.
//            if (len > tmp_len)
//                [ len := tmp_len ].
//             
//            self convert(i, &len, aLiteral, T<ByteArray>(tmp), &tmp_len).
//            
//            if (nil == aRetVal)
//                [ aRetVal := ByteArray new(0, tmp_len, T<ByteArray>(tmp)) ];
//                [ aRetVal := aRetVal add(tmp_len, T<ByteArray>(tmp)) ].
//            
//            i := i + len.
//            remaining := remaining - len
//        ].
//        
//        ^ aRetVal
//    ]
//    
//    toLiteral(int anIndex, int aLength, ByteArray anArray)
//    [
//        int     i := anIndex.
//        literal aRetVal := nil.
//        int     remaining := aLength.
//        
//        while (i < aLength)
//        [
//            V<char,64> tmp.
//            int        tmp_len := 64.
//            int        len := remaining.
//            if (len > tmp_len)
//                [ len := tmp_len ].
//             
//            self convert(i, &len, anArray, T<CharArray>(tmp), &tmp_len).
//            
//            if (nil == aRetVal)
//               [ aRetVal := LiteralValue new(0, tmp_len, T<CharArray>(tmp)) ];
//               [ aRetVal := aRetVal add(LiteralValue new(0, tmp_len, T<CharArray>(tmp))) ].
//            
//            i := i + len.
//            remaining := remaining - len.
//        ].
//        
//        ^ aRetVal.
//    ]
//    
//    toWide(int anIndex, int aLength, ByteArray anArray)
//    [
//        int  i := anIndex.
//        wide aRetVal := nil.
//        int  remaining := aLength.
//        
//        while (i < aLength)
//        [
//            V<char,64> tmp.
//            int        tmp_len := 64.
//            int        len := remaining.
//            if (len > tmp_len)
//                [ len := tmp_len ].
//             
//            self convert(i, &len, anArray, T<CharArray>(tmp), &tmp_len).
//            
//            if (nil == aRetVal)
//               [ aRetVal := WideLiteralValue new(0, tmp_len, T<CharArray>(tmp)) ];
//               [ aRetVal := aRetVal add(WideLiteralValue new(0, tmp_len, T<CharArray>(tmp))) ].
//            
//            i := i + len.
//            remaining := remaining - len
//        ].
//        
//        ^ aRetVal
//    ]
}

// --- UTF8Encoder ---

public singleton UTF8Encoder : Encoder
{
    convert(int index, ref int length, byte[] array, char[] output, ref int outputLength)
        => system'core_routines'__utf8encoder;
    
    convert(int index, ref int length, char[] array, byte[] output, ref int outputLength)
        => system'core_routines'__utf8encoder;
    
//    convert(int anIndex, ref<int> aLength, literal aLiteral, ByteArray anOutput, ref<int> anOutputLength)
//    [
//        int l := aLength value.
//        aLiteral saveToByteArray(anIndex, anOutput, l).
//
//        aLength readValueTo(anOutputLength).
//    ]
//    
//    toLiteral(int anIndex, int aLength, ByteArray anArray)
//        = UTF8Encoding toLiteral(anIndex, aLength, anArray).
}

//// --- UTF8Encoding ---
//
//public class UTF8Encoding =
//{
//    T<Encoder> encoder = UTF8Encoder.
//    
//    charCount(literal aLiteral, ref<int> aRetVal)
//        => system'core_routines'__utf8encoder.
//        
//    convert(int anIndex, int aLength, literal aLiteral, ByteArray anArray)
//    [
//        aLiteral saveToByteArray(anIndex, anArray, aLength).
//    ]
//        
//    toLiteral(int anIndex, int aLength, ByteArray anArray)
//        = LiteralValue fromByteArray(anIndex, aLength, anArray).
//        
//    convert(int anIndex, ref<int> aLength, ByteArray aByteArray, CharArray anOutput, ref<int> anOutputLength)
//        => system'core_routines'__utf8encoder.
//        
//    convert(int anIndex, ref<int> aLength, literal aByteArray, CharArray anOutput, ref<int> anOutputLength)
//        => system'core_routines'__utf8encoder.
//}.
//
//// --- UTF16Encoder ---
//
//public singleton UTF16Encoder :: Encoder
//{
//    convert(int anIndex, ref<int> aLength, ByteArray aByteArray, CharArray anOutput, ref<int> anOutputLength)
//        => system'core_routines'__utf16encoder.
//    
//    convert(int anIndex, ref<int> aLength, CharArray aByteArray, ByteArray anOutput, ref<int>anOutputLength)
//        => system'core_routines'__utf16encoder.
//}

// --- UTF16Encoding ---

public singleton UTF16Encoding
{
//    T<Encoder> encoder = UTF16Encoder.
//    
//    charCount(wide aLiteral, ref<int> aRetVal)
//        => system'core_routines'__utf16encoder.
//        
//    convert(int anIndex, int aLength, wide aLiteral, ShortArray anArray)
//    [
//        aLiteral saveToShortArray(anIndex, anArray, aLength).
//    ]
//
//    convert(int anIndex, int aSize, ShortArray aByteArray, CharArray anOutput, ref<int> anOutputLength)
//        => system'core_routines'__utf16encoder.
        
    convert(int index, int size, char[] array, short[] output, ref int outputLength)
        => system'core_routines'__utf16encoder;
        
//    convert(int anIndex, ref<int> aSize, literal aLiteral, ShortArray anOutput, ref<int> anOutputLength)
//    [
//        V<char,64> temp.
//        int        temp_len := 64.
//        if (temp_len < aSize)
//            [ aSize value := temp_len ].
//        
//        UTF8Encoding convert(anIndex, aSize, aLiteral, T<CharArray>(temp), &temp_len).
//        
//        self convert(0, temp_len, T<CharArray>(temp), anOutput, anOutputLength)
//    ]
//        
//    literal toLiteral(int anIndex, int aSize, ShortArray anInput)
//    [
//        V<char,64> temp.
//        int        temp_len := 64.
//        
//        self convert(anIndex, aSize, anInput, T<CharArray>(temp), &temp_len).
//        
//        ^ LiteralValue new(0, temp_len, T<CharArray>(temp))
//    ]
}
