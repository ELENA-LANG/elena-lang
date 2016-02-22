// ==== ver 3.4.1 ===

#import system. 

// === basic interfaces ===
#subject(class:system'io'Stream)stream.

#subject file_path.
#subject directory_path.

#class(extension)ioTypeOp
{
    #method(embeddable) file_path = self.
    
    #method(embeddable) directory_path = self.
}

// === i/o interfaces ===
#subject(class:system'io'TextReader)textreader.
#subject(class:system'io'BinaryReader)binaryreader.
#subject(class:system'io'TextWriter)textwriter.
#subject(class:system'io'BinaryWriter)binarywriter.

// --- Stream ---

#class(limited)Stream
{
    #method stream = $self.
    
    #method(stacksafe) writeLength &int:aLength [ ] // !! default implementation
        
    #method(stacksafe) readLength &vint:aLength [ aLength << 0. ] // !! default implementation
        
    #method(type:int,stacksafe,embeddable,suppress:w2) length
    [
        #var(type:int)aLength.
        $self readLength &vint:aLength.
        
        ^ IntNumber new &int:aLength.
    ]
    
    #method(stacksafe) set &length:length
    [
        $self writeLength &int:(length int).
    ]
    
    #method(stacksafe) readIndex &vint:anIndex []

    #method(type:int,stacksafe,embeddable,suppress:w2) index
    [
        #var(type:vint)anIndex.        
        $self readIndex &vint:anIndex.
        
        ^ anIndex.
    ]

    #method write &index:anIndex
    [
        $self writeIndex &int:(anIndex int).
    ]

    #method append &index:anIndex 
    [
        #var(type:int)aNewIndex.
        $self readIndex &vint:aNewIndex.
        
        aNewIndex := aNewIndex + anIndex int.
        
        $self writeIndex &int:aNewIndex.
    ]

    #method reduce &index:anIndex 
        <= reduceIndex &int:(anIndex int).
        
    #method(stacksafe) reduceIndex &int:anIndex 
    [
        #var(type:int)aNewIndex.
        $self readIndex &vint:aNewIndex.
        
        aNewIndex := aNewIndex - anIndex.
        
        $self writeIndex &int:aNewIndex.
    ]

    #method(stacksafe) writeIndex &int:anIndex []
        
    #method(stacksafe) read &bytearray:aDump &vint:aLength []
    
    #method(stacksafe) write &bytearray:aDump &int:aLength []
    
    #method close []
}
