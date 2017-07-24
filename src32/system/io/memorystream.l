// --- MemoryBuffer ---

const int $TextPageSize = 128.

type membuffer :: MemoryBuffer.

sealed class MemoryBuffer
{
    bytearray theBuffer.
    int       theUsed.
    int       theCapacity.
    
    constructor new int:length
    [
        theBuffer := ByteArray new int:length.
        theCapacity := length.
        theUsed := 0.
    ]
    
    constructor new
        <= new int:$TextPageSize.
    
    membuffer = $self.
    
    bytearray = theBuffer.
    
    stacksafe reserve int:len
    [
        theCapacity := theUsed + len.
        theCapacity := (theCapacity / $TextPageSize + 1) * $TextPageSize.
        
        bytearray aNewBuffer := ByteArray new int:theCapacity.
            
        aNewBuffer write int:0 int:theUsed bytearray:theBuffer.
            
        theBuffer := aNewBuffer.
    ]
    
    stacksafe $write int:index int:length bytearray:array
    [        
        byte temp(64).
        int  n := index.
        int  len := length.
        int  temp_len := 64.
        
        while (len > 0)
        [
            if(len < 64)
                [ temp_len := len ];
                [ temp_len := 64 ].
            
            array read int:n int:temp_len bytearray:temp.
            theBuffer write int:theUsed int:temp_len bytearray:temp.
            
            n := n + temp_len.
            len := len - temp_len
        ]
    ]
    
    stacksafe write int:index int:length bytearray:array
    [
        if(index > theUsed)
            [ OutOfRangeException new; raise ].
        
        if(theUsed + length > theCapacity)
            [ $self reserve int:length ].
            
        if(index == 0)
            [
                theBuffer write int:theUsed int:length bytearray:array.
                theUsed := theUsed + length
            ];
            [
                $self $write int:index int:length bytearray:array
            ]
    ]
    
    stacksafe read int:index vint:retVal
    [
        if(index > theUsed)
            [ OutOfRangeException new; raise ].
        
        theBuffer read int:index vint:retVal
    ]
    
    stacksafe delete int:anIndex int:aLength
    [
        int n := 0 - aLength.
        int l := theUsed.
        l := l - anIndex.
        l := l - aLength.
        
        theBuffer move int:(anIndex + aLength) int:l int:n.
        
        theUsed := theUsed - aLength.
    ]
    
    int length :: outnprop(readLengthTo).
    
    stacksafe readLengthTo vint:retVal
    [
        retVal int := theUsed.
    ]
}