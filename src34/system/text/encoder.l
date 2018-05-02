// === basic interfaces ===

// --- Encoder ---

public interface Encoder
{
    stacksafe abstract convert(int anIndex, ref<int> aLength, bytearray aByteArray, chararray anOutput, ref<int> anOutputLength) [] 

    stacksafe abstract convert(int anIndex, ref<int> aLength, chararray aCharArray, bytearray anOutput, ref<int> anOutputLength) []
    
//    stacksafe convert int:anIndex vint:aLength literal:aLiteral bytearray:anOutput vint:anOutputLength
//    [
//        char<64> temp.
//        int      temp_len := 64.
//        if (temp_len > aLength)
//            [ temp_len := aLength ];
//            [ aLength int := temp_len ].
//        
//        aLiteral Substring int:anIndex int:aLength; save int:0 chararray:temp vint:temp_len.
//        
//        $self convert int:0 vint:temp_len chararray:temp bytearray:anOutput vint:anOutputLength.
//    ]
//    
//    stacksafe convert int:anIndex vint:aLength wide:aLiteral bytearray:anOutput vint:anOutputLength
//    [
//        char<64> temp.
//        int      temp_len := 64.
//        if (temp_len > aLength)
//            [ temp_len := aLength ];
//            [ aLength int := temp_len ].
//        
//        aLiteral Substring int:anIndex int:aLength; save int:0 chararray:temp vint:temp_len.
//        
//        $self convert int:0 vint:temp_len chararray:temp bytearray:anOutput vint:anOutputLength.
//    ]
//    
//    stacksafe toByteArray int:anIndex int:aLength literal:aLiteral
//    [
//        int       i := anIndex.
//        bytearray aRetVal := $nil.
//        int       remaining := aLength.
//        
//        while (i < aLength)
//        [
//            byte<64> tmp.
//            int      tmp_len := 64.
//            int      len := remaining.
//            if (len > tmp_len)
//                [ len := tmp_len ].
//             
//            $self convert int:i vint:len literal:aLiteral bytearray:tmp vint:tmp_len.
//            
//            if ($nil == aRetVal)
//                [ aRetVal := ByteArray new int:0 int:tmp_len bytearray:tmp ];
//                [ aRetVal := aRetVal add int:tmp_len bytearray:tmp ].
//            
//            i := i + len.
//            remaining := remaining - len
//        ].
//        
//        ^ aRetVal
//    ]
//    
//    stacksafe toByteArray int:anIndex int:aLength wide:aLiteral
//    [
//        int       i := anIndex.
//        bytearray aRetVal := $nil.
//        int       remaining := aLength.
//        
//        while (i < aLength)
//        [
//            byte<64> tmp.
//            int      tmp_len := 64.
//            int      len := remaining.
//            if (len > tmp_len)
//                [ len := tmp_len ].
//             
//            $self convert int:i vint:len wide:aLiteral bytearray:tmp vint:tmp_len.
//            
//            if ($nil == aRetVal)
//                [ aRetVal := ByteArray new int:0 int:tmp_len bytearray:tmp ];
//                [ aRetVal := aRetVal add int:tmp_len bytearray:tmp ].
//            
//            i := i + len.
//            remaining := remaining - len
//        ].
//        
//        ^ aRetVal
//    ]
//    
//    stacksafe toLiteral int:anIndex int:aLength bytearray:anArray
//    [
//        int     i := anIndex.
//        literal aRetVal := $nil.
//        int     remaining := aLength.
//        
//        while (i < aLength)
//        [
//            char<64> tmp.
//            int      tmp_len := 64.
//            int      len := remaining.
//            if (len > tmp_len)
//                [ len := tmp_len ].
//             
//            $self convert int:i vint:len bytearray:anArray chararray:tmp vint:tmp_len.
//            
//            if ($nil == aRetVal)
//               [ aRetVal := LiteralValue new int:0 int:tmp_len chararray:tmp ];
//               [ aRetVal := aRetVal add literal:(LiteralValue new int:0 int:tmp_len chararray:tmp) ].
//            
//            i := i + len.
//            remaining := remaining - len.
//        ].
//        
//        ^ aRetVal.
//    ]
//    
//    stacksafe toWide int:anIndex int:aLength bytearray:anArray
//    [
//        int  i := anIndex.
//        wide aRetVal := $nil.
//        int  remaining := aLength.
//        
//        while (i < aLength)
//        [
//            char<64> tmp.
//            int      tmp_len := 64.
//            int      len := remaining.
//            if (len > tmp_len)
//                [ len := tmp_len ].
//             
//            $self convert int:i vint:len bytearray:anArray chararray:tmp vint:tmp_len.
//            
//            if ($nil == aRetVal)
//               [ aRetVal := WideLiteralValue new int:0 int:tmp_len chararray:tmp ];
//               [ aRetVal := aRetVal add wide:(WideLiteralValue new int:0 int:tmp_len chararray:tmp) ].
//            
//            i := i + len.
//            remaining := remaining - len
//        ].
//        
//        ^ aRetVal
//    ]
}

// --- UTF8Encoder ---

public singleton UTF8Encoder :: Encoder
{
    convert(int anIndex, ref<int> aLength, bytearray aByteArray, chararray anOutput, ref<int> anOutputLength)
        => system'core_routines'__utf8encoder.
    
    convert(int anIndex, ref<int> aLength, chararray aByteArray, bytearray anOutput, ref<int> anOutputLength)
        => system'core_routines'__utf8encoder.
    
//    stacksafe convert int:anIndex vint:aLength literal:aLiteral bytearray:anOutput vint:anOutputLength
//    [
//        aLiteral $save int:anIndex bytearray:anOutput int:aLength.
//
//        anOutputLength int := aLength
//    ]
//    
//    stacksafe toLiteral int:anIndex int:aLength bytearray:anArray
//        = UTF8Encoding toLiteral int:anIndex int:aLength bytearray:anArray.
}

// --- UTF8Encoding ---

class UTF8Encoding =
{
//    encoder = UTF8Encoder.
    
    stacksafe charCount(literal aLiteral, ref<int> aRetVal)
        => system'core_routines'__utf8encoder.
        
//    stacksafe convert int:anIndex int:aLength literal:aLiteral bytearray:anArray
//    [
//        aLiteral $save int:anIndex bytearray:anArray int:aLength.
//    ]
//        
//    stacksafe toLiteral int:anIndex int:aLength bytearray:anArray
//        = LiteralValue $new int:anIndex int:aLength bytearray:anArray.
        
    stacksafe convert(int anIndex, ref<int> aLength, bytearray aByteArray, chararray anOutput, ref<int> anOutputLength)
        => system'core_routines'__utf8encoder.
        
    stacksafe convert(int anIndex, ref<int> aLength, literal aByteArray, chararray anOutput, ref<int> anOutputLength)
        => system'core_routines'__utf8encoder.
}.

// --- UTF16Encoder ---

public singleton UTF16Encoder :: Encoder
{
    convert(int anIndex, ref<int> aLength, bytearray aByteArray, chararray anOutput, ref<int> anOutputLength)
        => system'core_routines'__utf16encoder.
    
    convert(int anIndex, ref<int> aLength, chararray aByteArray, bytearray anOutput, ref<int>anOutputLength)
        => system'core_routines'__utf16encoder.
}

// --- UTF16Encoding ---

public singleton UTF16Encoding
{
//    encoder = UTF16Encoder.
    
    stacksafe charCount(wide aLiteral, ref<int> aRetVal)
        => system'core_routines'__utf16encoder.
        
//    stacksafe convert int:anIndex int:aLength wide:aLiteral shortarray:anArray
//    [
//        aLiteral $save int:anIndex shortarray:anArray int:aLength.
//    ]

    stacksafe convert(int anIndex, int aSize, shortarray aByteArray, chararray anOutput, ref<int> anOutputLength)
        => system'core_routines'__utf16encoder.
        
    stacksafe convert(int anIndex, int aSize, chararray aByteArray, shortarray anOutput, ref<int> anOutputLength)
        => system'core_routines'__utf16encoder.
        
//    stacksafe convert int:anIndex vint:aSize literal:aLiteral shortarray:anOutput vint:anOutputLength
//    [
//        char<64> temp.
//        int      temp_len := 64.
//        if (temp_len < aSize)
//            [ aSize int := temp_len ].
//        
//        UTF8Encoding convert int:anIndex vint:aSize literal:aLiteral chararray:temp vint:temp_len.
//        
//        $self convert int:0 int:temp_len chararray:temp shortarray:anOutput vint:anOutputLength
//    ]
//        
//    stacksafe literal toLiteral int:anIndex int:aSize shortarray:anInput
//    [
//        char<64> temp.
//        int      temp_len := 64.
//        
//        $self convert int:anIndex int:aSize shortarray:anInput chararray:temp vint:temp_len.
//        
//        ^ LiteralValue new int:0 int:temp_len chararray:temp
//    ]
}
