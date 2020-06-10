namespace io
{
    // --- MemoryBuffer ---
    
    const int TextPageSize = 128;
    
    public sealed class MemoryBuffer
    {
        byte[]    theBuffer;
        ref<int>  theUsed;
        ref<int>  theCapacity;
        
        constructor allocate()
            <= allocate(TextPageSize);
        
        constructor allocate(int length)
        {
            theBuffer := new byte[](length);
            theCapacity := length;
            theUsed := 0;
        }
        
        get byte[] Value()
        {
            int used := theUsed.Value;
            byte[] b := new byte[](used);
            
            for (int i := 0, i < used, i+=1)
            {
                b[i] := theBuffer[i]
            };
            
            ^ b
        }
        
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
        
        int readInt32(int index)
        {
            int used := theUsed.Value;
            
            if(index > used)
                { OutOfRangeException.new().raise() };
            
            theBuffer.read(index, ref int retVal);
            
            ^ retVal
        }
        
        read(int index, int length, byte[] array)
        {
            int used := theUsed.Value;
            
            if(index + length > used)
                { OutOfRangeException.raise() };
                
            theBuffer.read(index, length, array)
        }
        
        delete(int index, int length)
        {
            int n := 0 - length;
            int l := theUsed.Value;
            
            l := l - index;
            l := l - length;
            
            theBuffer.move(index + length, l, n);
            
            theUsed.reduce(length)
        }
        
        int Length
        {
            get()
            {
                int retVal := theUsed.Value;
                
                ^ retVal
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
            theBuffer := MemoryBuffer.allocate();
            thePosition := 0
        }
        
        constructor(MemoryBuffer buffer)
        {
            theBuffer := buffer;
            thePosition := 0
        }
        
        constructor load(int index, int length, byte[] array)
        {
            self.write(array, index, length)
        }
        
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
    
        int Index
        {    
            get()
            {
                int retVal := thePosition.Value; 
                ^ retVal
            }
            
            set(int index)
            {
                int pos := theBuffer.Length;
                if (pos < index)
                    { OutOfRangeException.raise() };
                    
                thePosition.Value := index
            }
        }
        
        int Length
        {
            get()
            { 
                int len := theBuffer.Length;
            
                ^ len
            }
            
            set(int length)
            {
                NotSupportedException.raise()
            }
        }
        
        close()
        {
            thePosition.Value := 0;
            theBuffer.clear()
        }    
    }

}