#define system.

#symbol(const,type:int) $MB_ERR_INVALID_CHARS     = 00000008h.

//
//// --- AnsiEncoder ---
//
//#symbol(type:encoder) ansiEncoder = system'text'Encoder
//{
//    convert &bytearray:aByteArray &length:aLength &chararray:anOutput &out'length:anOutputLength
//        => system'internal'ws_copybuf.
//        
//    convert  &chararray:anOutput &length:anOutputLength &bytearray:aByteArray &out'length:aLength
//        => system'internal'ws_copy2buf.
//        
//    convert  &literal:anOutput &length:anOutputLength &bytearray:aByteArray &out'length:aLength
//        => system'internal'ws_copy2buf.
//}.

// --- WinEncoder ---

#class(struct) WinEncoder :: Encoder
{
    #field(type:int)theCodePage.
    
    #constructor new &int:aCodePage
    [
        theCodePage := aCodePage.
    ]

    #method convert &index:anIndex &vlength:aLength &bytearray:aByteArray &chararray:anOutput &vlength:anOutputLength 
    [
        (aLength > 127)
            ? [ #throw InvalidArgumentException new. ].
        
        #var(type:shortarray,size:127)temp_buf.
        #var(type:int)aConverted := aLength.
        #var(type:int)n := aLength.                
        
        (anIndex != 0)
            ? [
                #var(type:dirty_ptr)ptr.
                ptr write &bytearray:aByteArray.
                ptr append &int:anIndex.

                system'external'KERNEL32 MultiByteToWideChar &int:theCodePage &int:$MB_ERR_INVALID_CHARS &dirty_ptr:ptr &int:n &shortarray:temp_buf &vint:aConverted.
            ]
            ! [
                system'external'KERNEL32 MultiByteToWideChar &int:theCodePage &int:$MB_ERR_INVALID_CHARS &bytearray:aByteArray &int:n &shortarray:temp_buf &vint:aConverted.
            ].
        
        aConverted := aConverted * 2.
        UTF16Encoding $convert &index:0 &int:aConverted &shortarray:temp_buf &chararray:anOutput &vlength:anOutputLength.
    ]

    #method convert &index:anIndex &vlength:aLength &chararray:aByteArray &bytearray:anOutput &vlength:anOutputLength 
    [
    ]    
}

// --- Win1252Encoder ---

#static(type:encoder) WIN1252Encoder = WinEncoder new &int:1252.

// --- Win1251Encoder ---

#static(type:encoder) WIN1251Encoder = WinEncoder new &int:1251.