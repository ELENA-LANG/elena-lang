#import system.
#import system'math.
#import system'collections.
#import extensions.
#import extensions'math.

#class MemoryVirtual
{
    #field host.
    #field lookasideBuffer.
    #field pageDirectory.
    
    #constructor new &host:aHost
    [
        host := aHost.
        lookasideBuffer := Dictionary new.
        pageDirectory := 0.
    ]
    
    #method writeWord &address:aAddress &word:aWord
    [   
        #var page := aAddress and: 0FFFFF000h.
        #var blk := lookasideBuffer@page.
        ($nil == blk) 
            ? [ lookasideBuffer@page := ($self getPhysicalAddress:aAddress). ].
        host writeWord &address:aAddress &word:aWord.
    ]
    
    #method readWord : aAddress
    [
        #var page := aAddress and: 0FFFFF000h.
        #var blk := lookasideBuffer@page.
        ($nil == blk) 
            ? [ lookasideBuffer@page := ($self getPhysicalAddress:aAddress). ].
        ^(host readWord:(lookasideBuffer@page)).
    ]
    
    #method writeByte &address:aAddress &byte:aByte
    [   
        #var page := aAddress and: 0FFFFF000h.
        #var blk := lookasideBuffer@page.
        ($nil == blk) 
            ? [ lookasideBuffer@page := ($self getPhysicalAddress:aAddress). ].
        host writeByte &address:aAddress &byt:aByte.
    ]
    
    #method readByte : aAddress
    [
        #var page := aAddress and: 0FFFFF000h.
        #var blk := lookasideBuffer@page.
        ($nil == blk) 
            ? [ lookasideBuffer@page := ($self getPhysicalAddress:aAddress). ].
        ^(host readWord:(lookasideBuffer@page)).
    ]
    
    #method setPageDirectory : aAddress
    [
        pageDirectory := aAddress.
        //lookasideBuffer clear.
        lookasideBuffer := Dictionary new.
    ]
    
    #method getPhysicalAddress : aAddress
    [
        #var vaddr := aAddress / 01000h.
        #var table := host readWord:(pageDirectory + (vaddr / 1024)).
        ^(host readWord:(table + 4 * (vaddr mod:1024))).
    ]
}