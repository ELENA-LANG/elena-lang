// === basic interfaces ===

// --- Stream ---

public interface Stream
{
//    stream = $self.
    
//    int vprop length :: readLengthTo,writeLength.
    
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

//    abstract writeLength(int aLength) [ ]
//        
//    abstract readLengthTo(ref<int> aLength) [ ]

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
    
//    abstract write(ByteArray aDump, int aLength) []
    
    abstract close() {}
}
