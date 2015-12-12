#import system.
#import system'collections.

#class MemoryBlock
{
    #field memory.
    #constructor new
    [
        memory := ByteArray new &int:1000h.
    ]
    
    #method getAt : anIndex
    [
        ^memory@(anIndex and: 0FFFh).
    ]
    
    #method setAt : anIndex : anObject
    [
        memory@(anIndex and: 0FFFh) := anObject.
    ]
}