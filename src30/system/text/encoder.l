#define system. 

// === basic interfaces ===
#subject(class:system'text'Encoder)encoder.

// --- Encoder ---

#class(limited)Encoder
{
    #method encoder = $self.
    
    #method convert &bytearray:aByteArray &index:anIndex &length:aLength &chararray:anOutput &out'length:anOutputLength []
    
    #method convert &chararray:aCharArray &index:anIndex &length:aLength &bytearray:anOutput &out'length:anOutputLength []
}
