#define system.

// --- AnsiEncoder ---

#symbol(type:encoder) ansiLiteralEncoder = system'io'Encoder
{
    convert &bytearray:aByteArray &length:aLength &shortarray:anOutput &out'length:anOutputLength
        => system'internal'ws_copybuf.
        
    convert  &shortarray:anOutput &length:anOutputLength &bytearray:aByteArray &out'length:aLength
        => system'internal'ws_copy2buf.
        
    convert  &literal:anOutput &length:anOutputLength &bytearray:aByteArray &out'length:aLength
        => system'internal'ws_copy2buf.
}.
