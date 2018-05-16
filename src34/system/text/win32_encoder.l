const int MB_ERR_INVALID_CHARS     = 00000008h.

// --- WinEncoder ---

public struct WinEncoder :: Encoder
{
    int theCodePage.
    
    constructor new(int aCodePage)
    [
        theCodePage := aCodePage.
    ]

    convert(int anIndex, ref<int> aLength, ByteArray aByteArray, CharArray anOutput, ref<int> anOutputLength)
    [
        if (aLength > 127)
            [ InvalidArgumentException new; raise ].
        
        T<short, 127> temp_buf.
        int           aConverted := aLength.
        int           n := aLength.                
        
        if (anIndex != 0)
            [
                dirty_ptr ptr := /*0*/aByteArray.
//                ptr set:aByteArray.
                ptr offset(anIndex).

                aConverted := system'external'KERNEL32 MultiByteToWideChar(theCodePage, MB_ERR_INVALID_CHARS, ptr, n, T<ShortArray>(temp_buf), 127).
            ];
            [
                aConverted := system'external'KERNEL32 MultiByteToWideChar 
                    (theCodePage,
                    MB_ERR_INVALID_CHARS,
                    aByteArray,
                    n,
                    T<ShortArray>(temp_buf),
                    127).
            ].
        
        //aConverted *= 2.
        UTF16Encoding convert(0, aConverted, T<ShortArray>(temp_buf), anOutput, &anOutputLength).
    ]

    convert(int anIndex, ref<int> aLength, CharArray aByteArray, ByteArray anOutput, ref<int> anOutputLength)
    [
        T<short,127> temp_buf.
        int          temp_len := 127.
        int          chLen := aLength value.

        UTF16Encoding convert(anIndex, chLen, aByteArray, T<ShortArray>(temp_buf), &temp_len).
        
        int retVal := system'external'KERNEL32 WideCharToMultiByte(
            theCodePage, 0,
            T<ShortArray>(temp_buf),
            temp_len, 0, 0, 0, 0).

        if (retVal > chLen)
            [ InvalidArgumentException new; raise ].
                
        retVal := system'external'KERNEL32 WideCharToMultiByte(
            theCodePage, 0, T<ShortArray>(temp_buf), temp_len, anOutput, chLen, 0, 0).
        
        anOutputLength value := retVal
    ]    
}

// --- Win1252Encoder ---

public static T<Encoder> WIN1252Encoder = WinEncoder new(1252).

// --- Win1251Encoder ---

public static T<Encoder> WIN1251Encoder = WinEncoder new(1251).

// --- OEMEncoder ---

public static T<Encoder> OEMEncoder = WinEncoder new(1).

// --- AnsiEncoder ---

public static T<Encoder> AnsiEncoder = WinEncoder new(0).
