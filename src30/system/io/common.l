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
    
    #method readLength &vint:aLength [ aLength << 0. ] // !! default implementation
        
    #method length
    [
        #var(type:int)aLength.
        $self readLength &vint:aLength.
        
        ^ IntNumber new &int:aLength.
    ]
    
    #method readIndex &vint:anIndex []

    #method index
    [
        #var(type:vint)anIndex.
        
        $self readIndex &vint:anIndex.
        
        ^ IntNumber new &int:anIndex.
    ]

    #method write &index:anIndex []

    #method append &index:anIndex 
    [
        #var(type:vint)aNewIndex.
        $self readIndex &vint:aNewIndex.
        
        aNewIndex += anIndex.
        
        $self write &index:aNewIndex.
    ]

    #method reduce &index:anIndex 
    [
        #var(type:vint)aNewIndex.
        $self readIndex &vint:aNewIndex.
        
        aNewIndex -= anIndex.
        
        $self write &index:aNewIndex.
    ]

    #method writeIndex &int:anIndex []
        
    #method read &bytearray:aDump &vlength:aLength []
    
    #method write &bytearray:aDump &length:aLength []
    
    #method close []
}
