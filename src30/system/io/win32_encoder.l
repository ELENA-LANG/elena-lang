#define system.

// --- AnsiEncoder ---

#symbol ansiLiteralEncoder =
{
    read &type'bytearray:aByteArray &type'length:aLength
        = system'external'CoreAPI convert &type'bytearray:aByteArray &type'length:aLength &out'type'widestr:(LiteralValue new &type'length:aLength).
}.