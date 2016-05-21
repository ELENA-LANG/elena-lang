#import system.

#symbol(const,int) $MB_ERR_INVALID_CHARS     = 00000008h.

// --- WinEncoder ---

#class(struct) WinEncoder :: Encoder
{
    #field(int)theCodePage.
    
    #constructor new &int:aCodePage
    [
        theCodePage := aCodePage.
    ]

    #method convert &int:anIndex &vint:aLength &bytearray:aByteArray &chararray:anOutput &vint:anOutputLength 
    [
        (aLength > 127)
            ? [ #throw InvalidArgumentException new. ].
        
        #var(short:127)temp_buf.
        #var(int)aConverted := aLength.
        #var(int)n := aLength.                
        
        (anIndex != 0)
            ? [
                #var(dirty_ptr)ptr.
                ptr set:aByteArray.
                ptr offset &int:anIndex.

                aConverted := system'external'KERNEL32 MultiByteToWideChar &int:theCodePage &int:$MB_ERR_INVALID_CHARS &dirty_ptr:ptr &int:n &shortarray:temp_buf &int:127.
            ]
            ! [
                aConverted := system'external'KERNEL32 MultiByteToWideChar 
                    &int:theCodePage 
                    &int:$MB_ERR_INVALID_CHARS 
                    &bytearray:aByteArray 
                    &int:n 
                    &shortarray:temp_buf 
                    &int:127.
            ].
        
        //aConverted *= 2.
        UTF16Encoding convert &int:0 &int:aConverted &shortarray:temp_buf &chararray:anOutput &vint:anOutputLength.
    ]

    #method convert &int:anIndex &vint:aLength &chararray:aByteArray &bytearray:anOutput &vint:anOutputLength 
    [
        #var(short:127)temp_buf.
        #var(int)temp_len := 127.

        UTF16Encoding convert &int:anIndex &int:aLength &chararray:aByteArray &shortarray:temp_buf &vint:temp_len.
        
        #var(int)retVal := system'external'KERNEL32 WideCharToMultiByte 
            &int:theCodePage 
            &int:0
            &shortarray:temp_buf 
            &int:temp_len
            &int:0
            &int:0
            &int:0 &int:0.

        (retVal > anOutputLength)
            ? [ #throw InvalidArgumentException new. ].
                
        system'external'KERNEL32 WideCharToMultiByte 
            &int:theCodePage 
            &int:0 
            &shortarray:temp_buf 
            &int:temp_len
            &bytearray:anOutput
            &int:retVal
            &int:0 &int:0.
        
        anOutputLength := retVal.
    ]    
}

// --- Win1252Encoder ---

#static(encoder) WIN1252Encoder = WinEncoder new &int:1252.

// --- Win1251Encoder ---

#static(encoder) WIN1251Enc = WinEncoder new &int:1251.

// --- OEMEncoder ---

#static(encoder) OEMEncoder = WinEncoder new &int:1.

// --- AnsiEncoder ---

#static(encoder) AnsiEncoder = WinEncoder new &int:0.
