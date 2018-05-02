const int MB_ERR_INVALID_CHARS     = 00000008h.

// --- WinEncoder ---

public struct WinEncoder :: Encoder
{
    int theCodePage.
    
    constructor new(int aCodePage)
    [
        theCodePage := aCodePage.
    ]

    convert(int anIndex, ref<int> aLength, bytearray aByteArray, chararray anOutput, ref<int> anOutputLength)
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

                aConverted := system'external'KERNEL32 MultiByteToWideChar(theCodePage, MB_ERR_INVALID_CHARS, ptr, n, T<shortarray>(temp_buf), 127).
            ];
            [
                aConverted := system'external'KERNEL32 MultiByteToWideChar 
                    (theCodePage,
                    MB_ERR_INVALID_CHARS,
                    aByteArray,
                    n,
                    T<shortarray>(temp_buf),
                    127).
            ].
        
        //aConverted *= 2.
        UTF16Encoding convert(0, aConverted, T<shortarray>(temp_buf), anOutput, &anOutputLength).
    ]

    convert(int anIndex, ref<int> aLength, chararray aByteArray, bytearray anOutput, ref<int> anOutputLength)
    [
        T<short,127> temp_buf.
        int          temp_len := 127.

        UTF16Encoding convert(anIndex, aLength, aByteArray, T<shortarray>(temp_buf), &temp_len).
        
        int retVal := system'external'KERNEL32 WideCharToMultiByte(
            theCodePage, 0,
            T<shortarray>(temp_buf),
            temp_len, 0, 0, 0, 0).

        if (retVal > anOutputLength)
            [ InvalidArgumentException new; raise ].
                
        system'external'KERNEL32 WideCharToMultiByte(
            theCodePage, 0, T<shortarray>(temp_buf), temp_len, anOutput, retVal, 0, 0).
        
        anOutputLength value := retVal
    ]    
}

// --- Win1252Encoder ---

public static encoder WIN1252Encoder = WinEncoder new(1252).

// --- Win1251Encoder ---

public static encoder WIN1251Encoder = WinEncoder new(1251).

// --- OEMEncoder ---

public static encoder OEMEncoder = WinEncoder new(1).

// --- AnsiEncoder ---

public static encoder AnsiEncoder = WinEncoder new(0).
