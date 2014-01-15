#define system.

// --- AnsiEncoder ---

#symbol ansiLiteralEncoder =
{
    read &bytearray:aByteArray &length:aLength
        = primitive toLiteral &bytearray:aByteArray &length:aLength.
}.