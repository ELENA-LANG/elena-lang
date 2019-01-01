//const int MB_ERR_INVALID_CHARS     = 00000008h.

// --- WinEncoder ---

public struct WinEncoder : Encoder
{
    int theCodePage;
    
    constructor(int codePage)
    {
        theCodePage := codePage
    }

//    convert(int anIndex, ref<int> aLength, ByteArray aByteArray, CharArray anOutput, ref<int> anOutputLength)
//    [
//        if (aLength > 127)
//            [ InvalidArgumentException new; raise ].
//        
//        T<short, 127> temp_buf.
//        int           aConverted := aLength.
//        int           n := aLength.                
//        
//        if (anIndex != 0)
//            [
//                dirty_ptr ptr := /*0*/aByteArray.
//                
//                ptr := ptr + anIndex.
//
//                aConverted := system'external'KERNEL32 MultiByteToWideChar(theCodePage, MB_ERR_INVALID_CHARS, ptr, n, T<ShortArray>(temp_buf), 127).
//            ];
//            [
//                aConverted := system'external'KERNEL32 MultiByteToWideChar 
//                    (theCodePage,
//                    MB_ERR_INVALID_CHARS,
//                    aByteArray,
//                    n,
//                    T<ShortArray>(temp_buf),
//                    127).
//            ].
//        
//        //aConverted *= 2.
//        UTF16Encoding convert(0, aConverted, T<ShortArray>(temp_buf), anOutput, &anOutputLength).
//    ]

    convert(int index, ref int length, char[] array, byte[] output, ref int outputLength)
    {
        short temp_buf[127];
        int   temp_len := 127;
        int   chLen := length;

        UTF16Encoding.convert(index, chLen, array, temp_buf, ref temp_len);
        
        int retVal := extern KERNEL32.WideCharToMultiByte(
            theCodePage, 0,
            temp_buf,
            temp_len, 0, 0, 0, 0);

        if (retVal > chLen)
            { InvalidArgumentException.raise() };
                
        retVal := extern KERNEL32.WideCharToMultiByte(
            theCodePage, 0, temp_buf, temp_len, output, chLen, 0, 0);
        
        outputLength := retVal
    }
}

//// --- Win1252Encoder ---
//
//public static T<Encoder> WIN1252Encoder = WinEncoder new(1252).
//
//// --- Win1251Encoder ---
//
//public static T<Encoder> WIN1251Encoder = WinEncoder new(1251).

// --- OEMEncoder ---

public static Encoder OEMEncoder = new WinEncoder(1);

//// --- AnsiEncoder ---
//
//public static T<Encoder> AnsiEncoder = WinEncoder new(0).
