#define system. 

// === basic interfaces ===
#subject(class:system'text'Encoder)encoder.

// --- Encoder ---

#class(limited)Encoder
{
    #method encoder = $self.
    
    #method convert &index:anIndex &vlength:aLength &bytearray:aByteArray &chararray:anOutput &vlength:anOutputLength []

    #method convert &index:anIndex &vlength:aLength &chararray:aByteArray &bytearray:anOutput &vlength:anOutputLength []    
}

// --- UTF8Encoder ---

#symbol UTF8Encoder = Encoder
{
    #method convert &index:anIndex &vlength:aLength &bytearray:aByteArray &chararray:anOutput &vlength:anOutputLength
        => system'internal's_encode.
    
    #method convert &index:anIndex &vlength:aLength &chararray:aByteArray &bytearray:anOutput &vlength:anOutputLength
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
    #method convert &index:anIndex &vlength:aLength &bytearray:aByteArray &chararray:anOutput &vlength:anOutputLength
        => system'internal'ws_encode.
    
    #method convert &index:anIndex &vlength:aLength &chararray:aByteArray &bytearray:anOutput &vlength:anOutputLength
        => system'internal'ws_decode.
}.

// --- UTF16Encoding ---

#symbol UTF16Encoding =
{
    encoder = UTF16Encoder.
    
    charCount &wide:aLiteral &vint:aRetVal
        => system'internal'ws_charcount.
        
    $convert &index:anIndex &vlength:aLength &shortarray:aByteArray &chararray:anOutput &vlength:anOutputLength
        => system'internal'ws_encode.
}.
