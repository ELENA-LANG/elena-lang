// === basic interfaces ===
type stream       :: system'io'Stream.
type textreader   :: system'io'TextReader.
type binaryreader :: system'io'BinaryReader.
type textwriter   :: system'io'TextWriter.
type binarywriter :: system'io'BinaryWriter.

type file_path.
type directory_path.

extension ioTypeOp
{
    embeddable file_path = self.
    
    embeddable directory_path = self.
}

// --- Stream ---

limited class Stream
{
    stream = $self.
    
    int length :: nprop(readLengthTo,writeLength).
    
    int index :: nprop(readIndexTo,writeIndex).
    
    stacksafe writeLength int:aLength [ ] // !! default implementation
        
    stacksafe readLengthTo vint:aLength [ aLength int := 0. ] // !! default implementation
    
    stacksafe readIndexTo vint:anIndex []
    
    stacksafe writeIndex int:anIndex []

    appendIndex int:anIndex 
    [
        int aNewIndex := $self index.
        
        aNewIndex := aNewIndex + anIndex.
        
        $self writeIndex int:aNewIndex.
    ]

    reduce index:anIndex 
        <= reduceIndex int:anIndex.
        
    stacksafe reduceIndex int:anIndex 
    [
        int aNewIndex := $self index.
        
        aNewIndex := aNewIndex - anIndex.
        
        $self writeIndex int:aNewIndex.
    ]
        
    stacksafe read bytearray:aDump vint:aLength []
    
    stacksafe write bytearray:aDump int:aLength []
    
    close []
}
