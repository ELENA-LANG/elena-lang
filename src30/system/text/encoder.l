#define system. 

// === basic interfaces ===
#subject(class:system'text'Encoder)encoder.

// --- Encoder ---

#class(limited)Encoder
{
    #method encoder = $self.
    
    #method(stacksafe) convert &int:anIndex &vint:aLength &bytearray:aByteArray &chararray:anOutput &vint:anOutputLength []

    #method(stacksafe) convert &int:anIndex &vint:aLength &chararray:aByteArray &bytearray:anOutput &vint:anOutputLength []    
}

// --- UTF8Encoder ---

#symbol UTF8Encoder = Encoder
{
    #method convert &int:anIndex &vint:aLength &bytearray:aByteArray &chararray:anOutput &vint:anOutputLength
        => system'internal's_encode.
    
    #method convert &int:anIndex &vint:aLength &chararray:aByteArray &bytearray:anOutput &vint:anOutputLength
        => system'internal's_decode.
}.

// --- UTF8Encoding ---

#symbol UTF8Encoding =
{
    encoder = UTF8Encoder.
    
    charCount &literal:aLiteral &vint:aRetVal
        => system'internal's_charcount.
}.

// --- UTF16Encoder ---

#symbol UTF16Encoder = Encoder
{
    #method convert &int:anIndex &vint:aLength &bytearray:aByteArray &chararray:anOutput &vint:anOutputLength
        => system'internal'ws_encode.
    
    #method convert &int:anIndex &vint:aLength &chararray:aByteArray &bytearray:anOutput &vint:anOutputLength
        => system'internal'ws_decode.
}.

// --- UTF16Encoding ---

#symbol UTF16Encoding =
{
    encoder = UTF16Encoder.
    
    charCount &wide:aLiteral &vint:aRetVal
        => system'internal'ws_charcount.
        
    #method(stacksafe)$convert &int:anIndex &int:aSize &shortarray:aByteArray &chararray:anOutput &vint:anOutputLength
        => system'internal'ws_encode.
}.
