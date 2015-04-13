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

// --- Win1252Encoder ---

#symbol(type:encoder) WIN1252Encoder = Encoder
{
    #method convert &index:anIndex &vlength:aLength &bytearray:aByteArray &chararray:anOutput &vlength:anOutputLength 
    [
        (aLength > 127)
            ? [ #throw InvalidArgumentException new. ].
        
        #var(type:shortarray,size:127)temp_buf.
        #var(type:vint)aConverted := aLength.
        #var(type:int)n := aLength.                
        
        // !! when the variables are inside the sub code, the else block does not work correctly
        #var(type:int)nn.                
        #var(type:vint)nptr.
                        
        (anIndex != 0)
            ? [
                // !!  the code below should be overwritten
                #var(type:dirty_ptr)ptr.
                ptr write &bytearray:aByteArray.
                ptr append &int:anIndex.

                ptr read &vint:nptr.
                nn := nptr.
                                
                system'external'KERNEL32 MultiByteToWideChar &int:1252 &int:/*$MB_ERR_INVALID_CHARS*/8 &int:nn &int:n &shortarray:temp_buf &vint:aConverted.
            ]
            ! [
                system'external'KERNEL32 MultiByteToWideChar &int:1252 &int:/*$MB_ERR_INVALID_CHARS*/8 &bytearray:aByteArray &int:n &shortarray:temp_buf &vint:aConverted.
            ].
        
        aConverted := aConverted * 2.
        UTF16Encoding $convert &index:0 &vlength:aConverted &shortarray:temp_buf &chararray:anOutput &vlength:anOutputLength.
    ]

    #method convert &index:anIndex &vlength:aLength &chararray:aByteArray &bytearray:anOutput &vlength:anOutputLength 
    [
    ]    
}.