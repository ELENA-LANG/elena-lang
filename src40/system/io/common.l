// === basic interfaces ===

// --- Stream ---

public interface Stream
{
    prop int Length
    {
        abstract int get() {}
        
        abstract set(int retVal) {}
    }
    
    prop int Index
    {
        abstract get() {}
        
        abstract set(int retVal) {}
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
        
    abstract read(byte[] dump, ref int length) {}
    
    abstract write(byte[] dump, int length) {}
    
    abstract close() {}
}
