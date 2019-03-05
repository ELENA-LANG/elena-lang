// --- MemoryBuffer ---

const int TextPageSize = 128;

public sealed class MemoryBuffer
{
    byte[]    theBuffer;
    ref<int>  theUsed;
    ref<int>  theCapacity;
    
    constructor allocate(int length)
    {
        theBuffer := new byte[](length);
        theCapacity := length;
        theUsed := 0;
    }
    
    constructor()
        <= allocate(TextPageSize);
    
//    cast T<ByteArray> = ByteArray new(0, theUsed value, theBuffer).
    
    private reserve(int len)
    {
        int used := theUsed.Value;
        
        int capacity := used + len;
        capacity := (capacity / TextPageSize + 1) * TextPageSize;
        theCapacity.Value := capacity;
        
        auto newBuffer := new byte[](capacity);
            
        newBuffer.write(0, used, theBuffer);
            
        theBuffer := newBuffer
    }
    
    private writeInternal(int index, int length, byte[] array)
    {        
        byte temp[64];
        int  n := index;
        int  len := length;
        int  temp_len := 64;

        int used := theUsed.Value;
                
        while (len > 0)
        {
            if(len < 64)
            { 
                temp_len := len 
            }
            else
            { 
                temp_len := 64 
            };
            
            array.read(n, temp_len, temp);
            theBuffer.write(used, temp_len, temp);
            
            n := n + temp_len;
            len := len - temp_len
        }
    }
    
    write(int index, int length, byte[] array)
    {
        int used := theUsed.Value;        
        int capacity := theCapacity.Value;
        
        if(used < index)
            { OutOfRangeException.raise() };
        
        if(capacity < used + length)
            { self.reserve(length) };
            
        if(index == 0)
        {
            theBuffer.write(used, length, array);
            theUsed.append(length)
        }
        else
        {
            self.writeInternal(index, length, array)
        }
    }
    
//    read(int index, ref<int> retVal)
//    [
//        int used := 0.
//        theUsed readValueTo(&used).
//        
//        if(index > used)
//            [ OutOfRangeException new; raise ].
//        
//        theBuffer read(index, retVal)
//    ]
    
    read(int index, int length, byte[] array)
    {
        int used := theUsed.Value;
        
        if(index + length > used)
            { OutOfRangeException.raise() };
            
        theBuffer.read(index, length, array)
    }
    
//    delete(int index, int length)
//    [
//        int n := 0 - length.
//        int l := 0.
//        theUsed readValueTo(&l).
//        
//        l := l - index.
//        l := l - length.
//        
//        theBuffer move(index + length, l, n).
//        
//        theUsed reduce(length).
//    ]
//    
    vprop int Length
    {
        readTo(ref int retVal)
        {
            theUsed.readValueTo(ref retVal)
        }
    }

    clear()
    {
        theUsed.Value := 0
    }
}

// --- MemoryStream ---

public class MemoryStream : Stream
{
    MemoryBuffer theBuffer;
    ref<int>     thePosition;
    
    constructor()
    {
        theBuffer := new MemoryBuffer();
        thePosition := 0
    }
    
//    stacksafe constructor new int:anIndex int:aLength bytearray:anArray
//        <= new;
//    [
//        $self $write(anArray, anIndex, aLength).
//    ]
    
    write(byte[] dump, int length)
    {
        theBuffer.write(0, length, dump)
    }

    private write(byte[] dump, int index, int length)
    {
        theBuffer.write(index, length, dump)
    }

    read(byte[] dump, ref int len)
    {
        int pos := thePosition.Value;
        
        theBuffer.read(pos, len, dump);

        pos := len;

        thePosition.append(pos);
    }
    
    readIndexTo(ref int retVal)
    {
        thePosition.readValueTo(ref retVal)
    }
    
    writeIndex(int index)
    {
        int pos := theBuffer.Length;
        if (pos < index)
            { OutOfRangeException.raise() };
            
        thePosition.Value := index
    }
    
    readLengthTo(ref int retVal)
    { 
        int len := theBuffer.Length;
    
        retVal := len
    }
    
    writeLength(int length)
    {
        NotSupportedException.raise()
    }
    
    close()
    {
        thePosition.Value := 0;
        theBuffer.clear()
    }    
}