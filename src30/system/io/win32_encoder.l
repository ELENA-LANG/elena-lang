#define system.

// --- AnsiEncoder ---

#symbol ansiLiteralEncoder =
{
    read &bytearray:aByteArray &length:aLength
        = literalPrimitiveOp toLiteral &bytearray:aByteArray &length:aLength.
}.