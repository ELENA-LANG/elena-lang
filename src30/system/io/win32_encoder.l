#define system.

// --- AnsiEncoder ---

#symbol ansiLiteralEncoder =
{
    read &type'bytearray:aByteArray &type'length:aLength
        = strOp convert &type'bytearray:aByteArray &type'length:aLength.
}.