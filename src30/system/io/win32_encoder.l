#define system.

// --- AnsiEncoder ---

#symbol ansiLiteralEncoder =
{
    convert &bytearray:aByteArray &length:aLength &shortarray:anOutput &out'length:anOutputLength
        <ws_copybuf>.
        
    convert  &shortarray:anOutput &length:anOutputLength &bytearray:aByteArray &out'length:aLength
        <ws_copy2buf>.
        
    convert  &literal:anOutput &length:anOutputLength &bytearray:aByteArray &out'length:aLength
        <ws_copy2buf>.
}.