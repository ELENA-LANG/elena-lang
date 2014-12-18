#define system. 

// === basic interfaces ===
#type(class:system'io'Stream)stream.
#type(class:system'io'Encoder)encoder.

// === i/o interfaces ===
#type(class:system'io'TextReader)textreader.

// --- Stream ---

#class(limited,type:stream)Stream
{
    #method stream = $self.
    
    #method read &bytearray:aDump &length:aLength &out'length:aRead []
    
    #method close []
}

// --- Encoder ---

#class(limited,type:encoder)Encoder
{
    #method encoder = $self.

    #method convert &bytearray:aByteArray &length:aLength &shortarray:anOutput &out'length:anOutputLength []
        
    #method convert  &shortarray:anOutput &length:anOutputLength &bytearray:aByteArray &out'length:aLength []
        
    #method convert  &literal:anOutput &length:anOutputLength &bytearray:aByteArray &out'length:aLength []
}
