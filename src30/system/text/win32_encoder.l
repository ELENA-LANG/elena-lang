const int $MB_ERR_INVALID_CHARS     = 00000008h.

// --- WinEncoder ---

struct WinEncoder :: Encoder
{
    int theCodePage.
    
    constructor new int:aCodePage
    [
        theCodePage := aCodePage.
    ]

    convert int:anIndex vint:aLength bytearray:aByteArray chararray:anOutput vint:anOutputLength 
    [
        if (aLength > 127)
            [ InvalidArgumentException new; raise ].
        
        short temp_buf(127).
        int   aConverted := aLength.
        int   n := aLength.                
        
        if (anIndex != 0)
            [
                dirty_ptr ptr := 0.
                ptr set:aByteArray.
                ptr offset int:anIndex.

                aConverted := system'external'KERNEL32 MultiByteToWideChar int:theCodePage int:$MB_ERR_INVALID_CHARS dirty_ptr:ptr int:n shortarray:temp_buf int:127.
            ];
            [
                aConverted := system'external'KERNEL32 MultiByteToWideChar 
                    int:theCodePage 
                    int:$MB_ERR_INVALID_CHARS 
                    bytearray:aByteArray 
                    int:n 
                    shortarray:temp_buf 
                    int:127.
            ].
        
        //aConverted *= 2.
        UTF16Encoding convert int:0 int:aConverted shortarray:temp_buf chararray:anOutput vint:anOutputLength.
    ]

    convert int:anIndex vint:aLength chararray:aByteArray bytearray:anOutput vint:anOutputLength 
    [
        short temp_buf(127).
        int   temp_len := 127.

        UTF16Encoding convert int:anIndex int:aLength chararray:aByteArray shortarray:temp_buf vint:temp_len.
        
        int retVal := system'external'KERNEL32 WideCharToMultiByte 
            int:theCodePage 
            int:0
            shortarray:temp_buf 
            int:temp_len
            int:0
            int:0
            int:0 int:0.

        if (retVal > anOutputLength)
            [ InvalidArgumentException new; raise ].
                
        system'external'KERNEL32 WideCharToMultiByte 
            int:theCodePage 
            int:0 
            shortarray:temp_buf 
            int:temp_len
            bytearray:anOutput
            int:retVal
            int:0 int:0.
        
        anOutputLength int := retVal
    ]    
}

// --- Win1252Encoder ---

static encoder WIN1252Encoder = WinEncoder new int:1252.

// --- Win1251Encoder ---

static encoder WIN1251Enc = WinEncoder new int:1251.

// --- OEMEncoder ---

static encoder OEMEncoder = WinEncoder new int:1.

// --- AnsiEncoder ---

static encoder AnsiEncoder = WinEncoder new int:0.
