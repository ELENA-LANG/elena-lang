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

      abstract int read(byte[] dump, int offset, int length);

      abstract write(byte[] dump, int offset, int length);

      abstract close();
   }
}