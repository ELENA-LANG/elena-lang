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
      {
         int maxLength := length;
         int actualRead := 0;
         int actualOffset := offset;

         byte tempbuffer[1024];
         while (maxLength > 0) {
            int pageSize := (maxLength < 1024) ? maxLength : 1024;
            pageSize := read(tempbuffer, pageSize);
            if (pageSize == 0) { 
               :break
            };

            class Array<byte>.copy(dump, tempbuffer, actualOffset, pageSize);

            actualRead += pageSize;
            actualOffset += pageSize;

            maxLength += pageSize;
         };

         ^ actualRead;
      }
        
      write(byte[] dump, int offset, int length)
      {
         int actualLen := length;
         int actualOffset := offset;

         byte tempbuffer[1024];
         while (actualLen > 0) {
            int pageSize := (actualLen < 1024) ? actualLen : 1024;

            class Array<byte>.copy(tempbuffer, dump, actualOffset, pageSize);

            write(tempbuffer, pageSize);

            actualLen -= pageSize;
            actualOffset += pageSize;
         }
      }
        
      abstract close();
   }
}