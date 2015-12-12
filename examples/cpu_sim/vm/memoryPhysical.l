#import system.
#import system'collections.

#class MemoryPhysical
{
    #field allocatedMemory.
    #field wordReg.
    
    #constructor new
    [
        allocatedMemory := Dictionary new.
        wordReg := ByteArray new &int:4.
    ]
    
    #method writeWord &address:aAddress &word:aWord
    [
        #var bytes := $self getBytes &value:aWord.
        $self writeBytes &buffer:bytes &start:aAddress &offset:0 &size:4.
    ]
    
    #method readWord : address
    [
        $self readBytes &buffer:wordReg &start:address &offset:0 &size:4.
        ^($self getbits &value:wordReg &size:4).
    ]
   
    #method writeByte  &address:aAddress &byt:aByte
    [
        #var block := aAddress and: 0FFFFF000h.
        #var blk := allocatedMemory@block.
        ($nil == blk) 
            ? [ allocatedMemory@block := cpuvm'MemoryBlock new. ].
        (allocatedMemory@block)@aAddress := aByte.
    ]
    
    #method readByte : address
    [
        #var block := address and: 0FFFFF000h.
        #var blk := allocatedMemory@block.
        ($nil == blk) ? [ ^0FFh. ].
        ^(allocatedMemory@block)@address.
    ]
    
    #method readBytes &buffer:aBuf &start:aStart &offset:aOffset &size:aSize
    [
        #var i := 0.
        #loop (i<aSize) ?
        [
            aBuf@i := $self readByte:(aStart+i).
            i := i+1.
        ].
    ]

    #method writeBytes &buffer:aBuf &start:aStart &offset:aOffset &size:aSize
    [
        #var i := 0.
        #loop (i<aSize) ?
        [
            $self writeByte &address:(aStart + i) &byt:(aBuf@i).
            i := i+1.
        ].
    ]
    
    // --------    
    #method getbits &value:aValue &size:aSize
    [
        #var rtl := 0l.
        #var i := aSize - 1.
        #loop (i >= 0) ?
        [
            rtl := rtl shift &index:(-8).
            rtl := rtl or: (aValue@i).
            i := i - 1.
        ].
        ^rtl. 
    ]
    
    #method getBytes &value:aValue
    [
        #var i := 3.
        #var rtl := ByteArray new &int:4.
        #var(type:byte) a := 0.
        #loop (i >= 0)?
        [
            a := (aValue shift &index:(8*i)) and: 0FFh.
            rtl@i := a.
            i := i - 1.
        ].
        ^rtl.
    ]
}