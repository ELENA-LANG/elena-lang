#define system. 

// === basic interfaces ===
#subject(class:system'io'Stream)stream.

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
