// --- MemoryBuffer ---

const int TextPageSize = 128.

public sealed class MemoryBuffer
{
    T<ByteArray> theBuffer.
    ref<int>     theUsed.
    ref<int>     theCapacity.
    
    constructor new(int length)
    [
        theBuffer := ByteArray new(length).
        theCapacity := length.
        theUsed := 0.
    ]
    
    constructor new
        <= new(TextPageSize).
    
    cast T<ByteArray> = ByteArray new(0, theUsed value, theBuffer).
    
    private reserve(int len)
    [
        int used := 0.
        theUsed readValueTo(&used).
        
        int capacity := used + len.
        capacity := (capacity / TextPageSize + 1) * TextPageSize.        
        theCapacity value := capacity.        
        
        T<ByteArray> newBuffer := ByteArray new(capacity).
            
        newBuffer write(0, used, theBuffer).
            
        theBuffer := newBuffer.
    ]
    
    private writeInternal(int index, int length, ByteArray array)
    [        
        V<byte,64> temp.
        int        n := index.
        int        len := length.
        int        temp_len := 64.

        int used := 0.
        theUsed readValueTo(&used).
                
        while (len > 0)
        [
            if(len < 64)
                [ temp_len := len ];
                [ temp_len := 64 ].
            
            array read(n, temp_len, T<ByteArray>(temp)).
            theBuffer write(used, temp_len, T<ByteArray>(temp)).
            
            n := n + temp_len.
            len := len - temp_len
        ]
    ]
    
    write(int index, int length, ByteArray array)
    [
        int used := 0.
        theUsed readValueTo(&used).
        
        int capacity := 0.
        theCapacity readValueTo(&capacity).
        
        if(used < index)
            [ OutOfRangeException new; raise ].
        
        if(capacity < used + length)
            [ self reserve(length) ].
            
        if(index == 0)
            [
                theBuffer write(used, length, array).
                theUsed append(length)
            ];
            [
                self writeInternal(index, length, array)
            ]
    ]
    
    read(int index, ref<int> retVal)
    [
        int used := 0.
        theUsed readValueTo(&used).
        
        if(index > used)
            [ OutOfRangeException new; raise ].
        
        theBuffer read(index, retVal)
    ]
    
    read(int index, int length, ByteArray array)
    [
        int used := 0.
        theUsed readValueTo(&used).
        
        if(index + length > used)
            [ OutOfRangeException new; raise ].
            
        theBuffer read(index, length, array)
    ]
    
    delete(int index, int length)
    [
        int n := 0 - length.
        int l := 0.
        theUsed readValueTo(&l).
        
        l := l - index.
        l := l - length.
        
        theBuffer move(index + length, l, n).
        
        theUsed reduce(length).
    ]
    
    int rvprop length :: readLengthTo.
    
    readLengthTo(ref<int> retVal)
    [
        theUsed readValueTo(retVal).
    ]

    clear
    [
        theUsed value := 0.
    ]
}

// --- MemoryStream ---

public class MemoryStream :: Stream
{
    T<MemoryBuffer> theBuffer.
    ref<int>        thePosition.
    
    constructor new
    [
        theBuffer := MemoryBuffer new.
        thePosition := 0.
    ]
    
//    stacksafe constructor new int:anIndex int:aLength bytearray:anArray
//        <= new;
//    [
//        $self $write(anArray, anIndex, aLength).
//    ]
    
    write(ByteArray dump, int length)
    [
        theBuffer write(0, length, dump).
    ]

//    sealed $write bytearray:aDump int:anIndex int:aLength
//    [
//        theBuffer write int:anIndex int:aLength bytearray:aDump.
//    ]

    read(ByteArray dump, ref<int> len)
    [
        int pos := 0.
        thePosition readValueTo(&pos).
        
        theBuffer read(pos, len, dump).

        len readValueTo(&pos).

        thePosition append(pos).
    ]
    
    readIndexTo(ref<int> retVal)
    [
        thePosition readValueTo(retVal).
    ] 
    
    writeIndex(int index)
    [
        int pos := theBuffer length.
        if (pos < index)
            [ OutOfRangeException new; raise ].
            
        thePosition value := index.            
    ] 
    
    readLengthTo(ref<int> retVal)
    [ 
        int len := theBuffer length.
    
        retVal value := len. 
    ]
    
    writeLength(int length)
    [
        NotSupportedException new; raise
    ]
    
    close
    [
        thePosition value := 0.
        theBuffer clear.
    ]    
}