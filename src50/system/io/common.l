namespace io
{

    // === basic interfaces ===
    
    // --- Stream ---
    
    public interface Stream
    {
        abstract int Length
        {
            get();
            
            set(retVal);
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
            
        abstract read(byte[] dump, ref int length);
        
        abstract write(byte[] dump, int length);
        
        abstract close();
    }

}