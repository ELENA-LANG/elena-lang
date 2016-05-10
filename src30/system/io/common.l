#import system. 

// === basic interfaces ===
#subject stream       = system'io'Stream.
#subject textreader   = system'io'TextReader.
#subject binaryreader = system'io'BinaryReader.
#subject textwriter   = system'io'TextWriter.
#subject binarywriter = system'io'BinaryWriter.

#subject file_path.
#subject directory_path.

#class(extension)ioTypeOp
{
    #method(embeddable) file_path = self.
    
    #method(embeddable) directory_path = self.
}

// --- Stream ---

#class(limited)Stream
{
    #method stream = $self.
    
    #method(stacksafe,innprop:length) writeLength &int:aLength [ ] // !! default implementation
        
    #method(stacksafe,outnprop:length) readLength &vint:aLength [ aLength := 0. ] // !! default implementation
    
    #method(stacksafe,outnprop:index) readIndex &vint:anIndex []

    #method appendIndex &int:anIndex 
    [
        #var(int)aNewIndex := $self index.
        
        aNewIndex += anIndex.
        
        $self writeIndex &int:aNewIndex.
    ]

    #method reduce &index:anIndex 
        <= reduceIndex &int:(anIndex int).
        
    #method(stacksafe) reduceIndex &int:anIndex 
    [
        #var(int)aNewIndex := $self index.
        
        aNewIndex -= anIndex.
        
        $self writeIndex &int:aNewIndex.
    ]

    #method(stacksafe,innprop:index) writeIndex &int:anIndex []
        
    #method(stacksafe) read &bytearray:aDump &vint:aLength []
    
    #method(stacksafe) write &bytearray:aDump &int:aLength []
    
    #method close []
}
