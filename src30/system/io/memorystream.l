#import system.

// --- MemoryBuffer ---

#symbol(const,int)$TextPageSize = 128.

#subject membuffer = MemoryBuffer.

#class(sealed)MemoryBuffer
{
    #field(bytearray)theBuffer.
    #field(int)theUsed.
    #field(int)theCapacity.
    
    #constructor new &int:length
    [
        theBuffer := ByteArray new &int:length.
        theCapacity := length.
        theUsed := 0.
    ]
    
    #constructor new
        <= new &int:$TextPageSize.
    
    #method membuffer = $self.
    
    #method bytearray = theBuffer.
    
    #method(stacksafe,suppress:w3) reserve &int:len
    [
        theCapacity := theUsed + len.
        theCapacity := (theCapacity / $TextPageSize + 1) * $TextPageSize.
        
        #var(bytearray) aNewBuffer := ByteArray new &int:theCapacity.
            
        aNewBuffer write &int:0 &int:theUsed &bytearray:theBuffer.
            
        theBuffer := aNewBuffer.
    ]
    
    #method(stacksafe) $write &int:index &int:length &bytearray:array
    [        
        #var(byte:64)temp.
        #var(int)n := index.
        #var(int)len := length.
        #var(int)temp_len := 64.
        
        #loop (len > 0)?
        [
            (len < 64)
                ? [ temp_len := len. ]
                ! [ temp_len := 64. ].
            
            array read &int:n &int:temp_len &bytearray:temp.
            theBuffer write &int:theUsed &int:temp_len &bytearray:temp.
            
            n := n + temp_len.
            len := len - temp_len.
        ].
    ]
    
    #method(stacksafe,suppress:w3) write &int:index &int:length &bytearray:array
    [
        (index > theUsed)
            ? [ #throw OutOfRangeException new. ].
        
        (theUsed + length > theCapacity)?
            [ $self reserve &int:length. ].
            
        (index == 0)
            ? [
                theBuffer write &int:theUsed &int:length &bytearray:array.
                theUsed := theUsed + length.
            ]
            ! [
                $self $write &int:index &int:length &bytearray:array
            ].
    ]
    
    #method(stacksafe)read &int:index &vint:retVal
    [
        (index > theUsed)
            ? [ #throw OutOfRangeException new. ].
        
        theBuffer read &int:index &vint:retVal.
    ]
    
    #method(stacksafe,suppress:w3) delete &int:anIndex &int:aLength
    [
        #var(int)n := 0 - aLength.
        #var(int)l := theUsed.
        l := l - anIndex.
        l := l - aLength.
        
        theBuffer move &int:(anIndex + aLength) &int:l &int:n.
        
        theUsed := theUsed - aLength.
    ]
    
    #method(stacksafe,outnprop:length)readLength &vint:retVal
    [
        retVal := theUsed.
    ]
}