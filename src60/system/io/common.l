namespace io
{
   // --- Stream ---
   public interface Stream
   {
      abstract int Length
      {
         get();
      }
        
      abstract int Index
      {
         get();
            
         set(retVal);
      }
    
      appendIndex(int offs)
      {
         int newIndex := self.Index;
            
         newIndex += offs;
            
         self.Index := newIndex
      }
            
      reduceIndex(int offs)
      {
         int newIndex := self.Index;
            
         newIndex -= offs;
            
         self.Index := newIndex
      }
            
      abstract int read(byte[] dump, int length);
        
      abstract write(byte[] dump, int length);

      int read(byte[] dump, int offset, int length)
         = StreamOperations.read(self, dump, offset, length);

      write(byte[] dump, int offset, int length)
      {
         StreamOperations.write(self, dump, offset, length)
      }

      abstract close();
   }

   // Temporary arrays must stay outside the interface implementation
   internal singleton StreamOperations
   {
      int read(Stream stream, byte[] dump, int offset, int length)
      {
         if (offset < 0 || length < 0 || offset > dump.Length - length)
            { OutOfRangeException.raise() };

         int remaining := length;
         int actualRead := 0;
         int destIndex := offset;

         byte[] tempBuffer := new byte[](1024);
         while (remaining > 0) {
            int pageSize := (remaining < 1024) ? remaining : 1024;
            int read := stream.read(tempBuffer, pageSize);
            if (read == 0)
               { :break };

            Array<byte>.copyTo(dump, tempBuffer, destIndex, read);

            actualRead += read;
            destIndex += read;
            remaining -= read
         };

         ^ actualRead
      }

      write(Stream stream, byte[] dump, int offset, int length)
      {
         if (offset < 0 || length < 0 || offset > dump.Length - length)
            { OutOfRangeException.raise() };

         if (offset == 0) {
            stream.write(dump, length)
         }
         else {
            stream.write(class Array<byte>.copy(dump, offset, length), length)
         }
      }
   }
}
