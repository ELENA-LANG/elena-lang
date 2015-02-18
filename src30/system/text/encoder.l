#define system. 

// === basic interfaces ===
#subject(class:system'text'Encoder)encoder.

// --- Encoder ---

#class(limited)Encoder
{
    #method encoder = $self.

    #method convert &bytearray:aByteArray &length:aLength &chararray:anOutput &out'length:anOutputLength []
        
    #method convert &chararray:anOutput &length:anOutputLength &bytearray:aByteArray &out'length:aLength []
    
    #method convert &literal:anOutput &length:anOutputLength &bytearray:aByteArray &out'length:aLength []    
}
