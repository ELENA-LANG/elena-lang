// === basic interfaces ===

// --- Stream ---

public interface Stream
{
    vprop int Length
    {
        read(ref int retVal)
        {
            self.readLengthTo(ref retVal)
        }
        
        write(int retVal)
        {
            self.writeLength(retVal)
        }
    }
    
    vprop int Index
    {
        read(ref int retVal)
        {
            self.readIndexTo(ref retVal)
        }
        
        write(int retVal)
        {
            self.writeIndex(retVal)
        }
    }

    /// Reads the current index into an output parameter
    abstract readIndexTo(ref int retVal) {}

    /// Sets the index by specified value
    abstract writeIndex(int anIndex) {}

    abstract writeLength(int length) {}
        
    abstract readLengthTo(ref int length) {}

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
