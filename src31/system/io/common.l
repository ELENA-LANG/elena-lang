#import system. 

// === basic interfaces ===
#subject stream       = system'io'Stream.
#subject textreader   = system'io'TextReader.
#subject binaryreader = system'io'BinaryReader.
#subject textwriter   = system'io'TextWriter.
#subject binarywriter = system'io'BinaryWriter.

#subject file_path.
#subject directory_path.

extension ioTypeOp
{
    embeddable file_path = self.
    
    embeddable directory_path = self.
}

// --- Stream ---

limited class Stream
{
    stream = $self.
    
    stacksafe innprop(length) writeLength &int:aLength [ ] // !! default implementation
        
    stacksafe outnprop(length) readLength &vint:aLength [ aLength := 0. ] // !! default implementation
    
    stacksafe outnprop(index) readIndex &vint:anIndex []

    appendIndex &int:anIndex 
    [
        int aNewIndex := $self index.
        
        aNewIndex += anIndex.
        
        $self writeIndex &int:aNewIndex.
    ]

    reduce &index:anIndex 
        <= reduceIndex &int:(anIndex int).
        
    stacksafe reduceIndex &int:anIndex 
    [
        int aNewIndex := $self index.
        
        aNewIndex -= anIndex.
        
        $self writeIndex &int:aNewIndex.
    ]

    stacksafe innprop(index) writeIndex &int:anIndex []
        
    stacksafe read &bytearray:aDump &vint:aLength []
    
    stacksafe write &bytearray:aDump &int:aLength []
    
    close []
}
