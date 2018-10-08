// === basic interfaces ===

// --- Stream ---

public interface Stream
{
//    stream = $self.
    
    int vprop length :: readLengthTo,writeLength.
    
    int vprop index :: readIndexTo,writeIndex.
    
    abstract writeLength(int aLength) [ ]
        
    abstract readLengthTo(ref<int> aLength) [ ]
    
    abstract readIndexTo(ref<int> anIndex) []
    
    abstract writeIndex(int anIndex) []

    appendIndex(int anIndex)
    [
        int aNewIndex := self index.
        
        aNewIndex := aNewIndex + anIndex.
        
        self writeIndex(aNewIndex).
    ]
        
    reduceIndex(int anIndex)
    [
        int aNewIndex := self index.
        
        aNewIndex := aNewIndex - anIndex.
        
        self writeIndex(aNewIndex).
    ]
        
    abstract read(ByteArray aDump, ref<int> aLength) []
    
    abstract write(ByteArray aDump, int aLength) []
    
    abstract close []
}
