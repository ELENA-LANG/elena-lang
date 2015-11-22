// ==== ver 3.4.1 ===

#import system. 

// === basic interfaces ===
#subject(class:system'io'Stream)stream.
#subject(class:system'io'File) file.

// === i/o interfaces ===
#subject(class:system'io'TextReader)textreader.
#subject(class:system'io'BinaryReader)binaryreader.
#subject(class:system'io'TextWriter)textwriter.
#subject(class:system'io'BinaryWriter)binarywriter.

// --- Stream ---

#class(limited)Stream
{
    #method stream = $self.
    
    #method(stacksafe) readLength &vint:aLength [ aLength << 0. ] // !! default implementation
        
    #method length
    [
        #var(type:int)aLength.
        $self readLength &vint:aLength.
        
        ^ IntNumber new &int:aLength.
    ]
    
    #method(stacksafe) readIndex &vint:anIndex []

    #method index
    [
        #var(type:vint)anIndex.
        
        $self readIndex &vint:anIndex.
        
        ^ IntNumber new &int:anIndex.
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
