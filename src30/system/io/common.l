#define system. 

// === basic interfaces ===
#type(class:system'io'Stream)stream.
#type(class:system'io'Encoder)encoder.

// === i/o interfaces ===
#type(class:system'io'TextReader)textreader.
#type(class:system'io'BinaryReader)binaryreader.
#type(class:system'io'TextWriter)textwriter.
#type(class:system'io'BinaryWriter)binarywriter.

// --- Stream ---

#class(limited)Stream
{
    #method stream = $self.
    
    #method read &bytearray:aDump &length:aLength &out'int:aRead []
    
    #method write &bytearray:aDump &length:aLength []
    
    #method close []
}

// --- Encoder ---

#class(limited)Encoder
{
    #method encoder = $self.

    #method convert &bytearray:aByteArray &length:aLength &shortarray:anOutput &out'length:anOutputLength []
        
    #method convert  &shortarray:anOutput &length:anOutputLength &bytearray:aByteArray &out'length:aLength []
        
    #method convert  &literal:anOutput &length:anOutputLength &bytearray:aByteArray &out'length:aLength []
}
