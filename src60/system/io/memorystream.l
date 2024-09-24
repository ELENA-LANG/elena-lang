namespace io
{
   // --- MemoryBuffer ---
    
   const int TextPageSize = 128;
    
   public sealed class MemoryBuffer
   {
      byte[]         _buffer;
      Reference<int> _used;
      Reference<int> _capacity;
        
      constructor allocate()
         <= allocate(TextPageSize);
        
      constructor allocate(int length)
      {
         _buffer := new byte[](length);
         _capacity := length;
         _used := 0;
      }
        
      get byte[] Value()
      {
         int used := _used.Value;
         byte[] b := new byte[](used);
            
         for (int i := 0; i < used; i+=1)
         {
            b[i] := _buffer[i]
         };
            
         ^ b
      }
        
      private reserve(int len)
      {
         int used := _used.Value;
            
         int capacity := used + len;
         capacity := (capacity / TextPageSize + 1) * TextPageSize;
         _capacity.Value := capacity;
            
         auto newBuffer := new byte[](capacity);
                
         Array<byte>.copy(newBuffer, _buffer, 0, used);
                
         _buffer := newBuffer
      }
        
      private writeInternal(int index, int length, byte[] array)
      {        
         byte temp[64];
         int  n := index;
         int  len := length;
         int  temp_len := 64;
    
         int used := _used.Value;
                    
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
                
            Array<byte>.copy(temp, array, n, temp_len);
            Array<byte>.copyTo(_buffer, temp, used, temp_len);
                
            n := n + temp_len;
            len := len - temp_len
         }
      }
        
      write(int index, int length, byte[] array)
      {
         int used := _used.Value;        
         int capacity := _capacity.Value;
            
         if(used < index)
            { OutOfRangeException.raise() };
            
         if(capacity < used + length)
            { self.reserve(length) };
                
         if(index == 0)
         {
            Array<byte>.copyTo(_buffer, array, used, length);
            _used.append(length)
         }
         else
         {
            self.writeInternal(index, length, array)
         }
      }
        
      int readInt32(int index)
      {
         int used := _used.Value;
            
         if(index + 4 > used)
            { OutOfRangeException.new().raise() };
            
         PrimitiveIntOperations.loadFromByteArray(_buffer, index, out int retVal);
            
         ^ retVal
      }
        
      read(int index, int length, byte[] array)
      {
         int used := _used.Value;
            
         if(index + length > used)
            { OutOfRangeException.raise() };
                
         Array<byte>.copy(array, _buffer, index, length);
      }
        
      delete(int index, int length)
      {
         int n := 0 - length;
         int l := _used.Value;
            
         l := l - index;
         l := l - length;
            
         Array<byte>.move(_buffer, index + length, l, n);

         _used.reduce(length)
      }
        
      int Length
      {
         get()
         {
            int retVal := _used.Value;
                
            ^ retVal
         }
      }
    
      clear()
      {
         _used.Value := 0
      }
   }
    
   // --- MemoryStream ---
    
   public class MemoryStream : Stream
   {
      MemoryBuffer   _buffer;
      Reference<int> _position;
        
      constructor()
      {
         _buffer := MemoryBuffer.allocate();
         _position := 0
      }
        
      constructor(MemoryBuffer buffer)
      {
         _buffer := buffer;
         _position := 0
      }
        
      constructor load(int index, int length, byte[] array)
      {
         self.write(array, index, length)
      }
        
      write(byte[] dump, int length)
      {
         _buffer.write(0, length, dump)
      }
    
      private write(byte[] dump, int index, int length)
      {
         _buffer.write(index, length, dump)
      }
    
      int read(byte[] dump, int len)
      {
         int pos := _position.Value;
            
         _buffer.read(pos, len, dump);
    
         _position.append(len);

         ^ len
      }
    
      int Index
      {    
         get()
         {
            int retVal := _position.Value; 
            ^ retVal
         }
            
         set(int index)
         {
            int pos := _buffer.Length;
            if (pos < index)
               { OutOfRangeException.raise() };
                    
            _position.Value := index
         }
      }
        
      int Length
      {
         get()
         { 
            int len := _buffer.Length;
            
            ^ len
         }
            
         set(int length)
         {
            NotSupportedException.raise()
         }
      }
        
      close()
      {
         _position.Value := 0;
         _buffer.clear()
      }    
   }
}