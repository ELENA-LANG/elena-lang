#define system. 

// === basic interfaces ===
#subject(class:system'io'Stream)stream.
#subject(class:system'io'Encoder)encoder.

// === i/o interfaces ===
#subject(class:system'io'TextReader)textreader.
#subject(class:system'io'BinaryReader)binaryreader.
#subject(class:system'io'TextWriter)textwriter.
#subject(class:system'io'BinaryWriter)binarywriter.

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
